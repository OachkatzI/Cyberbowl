// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "Character/CBCharacterMovementComponent.h"

#include "Character/MovementStates/DashState.h"
#include "Character/MovementStates/WallrunState.h"
#include "Character/MovementStates/JumpState.h"
#include "Character/MovementStates/DoubleJumpState.h"
#include "Engine/Engine.h"
#include "Character/CyberbowlCharacterAnimInstance.h"
#include "Components/SphereComponent.h"
#include "Character/BoopComponent.h"

void UCBCharacterMovementComponent::SetCBMovementMode(ECBMovementMode mode)
{
	if(MovementStates.Num() == 0)
	{
        return;
	}
    MovementStates[CBMovementMode]->Deactivate();

    auto previousMode = CBMovementMode;
    CBMovementMode = mode;
	
	MovementStates[CBMovementMode]->Activate(previousMode);
	
    //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Emerald, FString::Printf(TEXT("MoveMode: %i"), CBMovementMode));
}

void UCBCharacterMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                  FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    MovementStates[CBMovementMode]->OnTick(DeltaTime);
}

void UCBCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();

    WallrunCollider = Cast<USphereComponent>(GetOwner()->GetComponentsByTag(USphereComponent::StaticClass(), "WallrunCollider").Last());
    auto boopComponent = CharacterOwner->FindComponentByClass<UBoopComponent>();
    if (!WallrunCollider || !boopComponent)
    {
        UE_LOG(LogInit, Error, TEXT("CBCharacterMoveComponent: Vital Components not set in Character Blueprint! (WallrunCollider, BoopComponent)"));
    }
    else
    {
        WallrunCollider->OnComponentBeginOverlap.AddDynamic(this, &UCBCharacterMovementComponent::CheckForWallrun);
        WallrunCollider->OnComponentEndOverlap.AddDynamic(this, &UCBCharacterMovementComponent::EndWallrun);
        boopComponent->OnBoopStarted.AddDynamic(this, &UCBCharacterMovementComponent::ForceEndWallrun);
    }

    CBMovementMode = ECBMovementMode::CBMOVE_Running;

    MovementStates.Add(ECBMovementMode::CBMOVE_Running, NewObject<UBaseMovementState>());
    MovementStates.Add(ECBMovementMode::CBMOVE_Jump, NewObject<UJumpState>());
    MovementStates.Add(ECBMovementMode::CBMOVE_DoubleJump, NewObject<UDoubleJumpState>());
    MovementStates.Add(ECBMovementMode::CBMOVE_Wallrun, NewObject<UWallrunState>());

    auto dashState = NewObject<UDashState>();
    dashState->OnUpDash.AddDynamic(this, &UCBCharacterMovementComponent::CallOnVerticalDash);
    MovementStates.Add(ECBMovementMode::CBMOVE_Dash, dashState);

	for(auto state : MovementStates)
	{
        state.Value->InitializeState(this);
	}

    animinstance = Cast<UCyberbowlCharacterAnimInstance>(GetOwner()->FindComponentByClass<USkeletalMeshComponent>()->GetAnimInstance());
    if (animinstance)
    {
        animinstance->SetDashPlayRate(DashDuration);
        animinstance->SetDoubleJumpPlayRate(DoubleJumpDuration);
    }
}

void UCBCharacterMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
    UCharacterMovementComponent::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);

	if(MovementMode == EMovementMode::MOVE_Walking)
	{
        SetCBMovementMode(ECBMovementMode::CBMOVE_Running);
	}
}

void UCBCharacterMovementComponent::ForceEndWallrun()
{
    if (GetCBMovementMode() == ECBMovementMode::CBMOVE_Wallrun)
    {
        auto movementState = Cast<UWallrunState>(GetCBMovementState());
        FVector launchVector = movementState->GetLaunchVector();
        launchVector = launchVector.GetSafeNormal() * 1000.f;
        AddImpulse(launchVector, true);
    }

    EndWallrun(nullptr, nullptr, nullptr, 0);
}

void UCBCharacterMovementComponent::CheckForWallrun(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                                    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (GetCBMovementMode() != ECBMovementMode::CBMOVE_Running
        && MovementMode != EMovementMode::MOVE_Walking)
    {
        //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Emerald, FString::Printf(TEXT("WallrunCmp valid Wallrun")));

        //push character against the wall 
        AddImpulse(SweepResult.Normal * 1000.f);

		SetCBMovementMode(ECBMovementMode::CBMOVE_Wallrun);
        OnWallrunStart.Broadcast();
    }

}

void UCBCharacterMovementComponent::EndWallrun(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    bool isLaunched = false;
    float timeOnWall = 0.f;
    //This check is necessary to avoid bugs when character just walks up to a wall 
    if (GetCBMovementMode() == ECBMovementMode::CBMOVE_Wallrun)
    {
        auto movementState = Cast<UWallrunState>(GetCBMovementState());
        timeOnWall = movementState->GetTimeOnWall();
        isLaunched = movementState->IsLaunching();
        if (!isLaunched)
        {
            SetCBMovementMode(ECBMovementMode::CBMOVE_Jump);
        }
    }
	OnWallrunEnd.Broadcast(timeOnWall, isLaunched);
}


/*
 *
 *
 * EXPERIMENTAL MOVEMENT
 * Experiment trying to add custom Movementmodes to the existing MovementMode System of Unreal
 * Is quite complex and makes my brain go ouchie
 * 
 */

void UCBCharacterMovementComponent::PhysCustom(float deltaTime, int32 Iterations)
{
	switch(CustomMovementMode)
	{
	case int(ECBMovementMode::CBMOVE_Wallrun) :
		PhysWallrun(deltaTime, Iterations);
		break;
	case int(ECBMovementMode::CBMOVE_Running) :
	default:
		break;
	}
}

void UCBCharacterMovementComponent::PhysWallrun(float deltaTime, int32 Iterations)
{
	Super::PhysCustom(deltaTime, Iterations);

    if (deltaTime < MIN_TICK_TIME)
    {
        return;
    }

    RestorePreAdditiveRootMotionVelocity();

    if (!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
    {
        if (bCheatFlying && Acceleration.IsZero())
        {
            Velocity = FVector::ZeroVector;
        }
        const float Friction = GroundFriction;//0.5f * GetPhysicsVolume()->FluidFriction;
        CalcVelocity(deltaTime, Friction, true, GetMaxBrakingDeceleration());
    }

    ApplyRootMotionToVelocity(deltaTime);

    Iterations++;
    bJustTeleported = false;

    FVector OldLocation = UpdatedComponent->GetComponentLocation();
    const FVector Adjusted = Velocity * deltaTime;
    FHitResult Hit(1.f);
    SafeMoveUpdatedComponent(Adjusted, UpdatedComponent->GetComponentQuat(), true, Hit);

    if (Hit.Time < 1.f)
    {
        const FVector GravDir = FVector(0.f, 0.f, -1.f);
        const FVector VelDir = Velocity.GetSafeNormal();
        const float UpDown = GravDir | VelDir;

        bool bSteppedUp = false;
        if ((FMath::Abs(Hit.ImpactNormal.Z) < 0.2f) && (UpDown < 0.5f) && (UpDown > -0.2f) && CanStepUp(Hit))
        {
            float stepZ = UpdatedComponent->GetComponentLocation().Z;
            bSteppedUp = StepUp(GravDir, Adjusted * (1.f - Hit.Time), Hit);
            if (bSteppedUp)
            {
                OldLocation.Z = UpdatedComponent->GetComponentLocation().Z + (OldLocation.Z - stepZ);
            }
        }

        if (!bSteppedUp)
        {
            //adjust and try again
            HandleImpact(Hit, deltaTime, Adjusted);
            SlideAlongSurface(Adjusted, (1.f - Hit.Time), Hit.Normal, Hit, true);
        }
    }

    //if(CurrentFloor.IsWalkableFloor() && CurrentFloor.HitResult.bStartPenetrating)
    //{
    //    SetMovementMode(MOVE_Walking);
    //}

    if (!bJustTeleported && !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
    {
        Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / deltaTime;
    }

}

void UCBCharacterMovementComponent::CallOnVerticalDash()
{
    OnVertDash.Broadcast();
}