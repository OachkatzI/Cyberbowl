#include "Character/MovementStates/WallrunState.h"
#include "Character/CBCharacterMovementComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/Character.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Character/CyberbowlCharacterAnimInstance.h"
#include "Engine/Engine.h"
#include "TimerManager.h"

UWallrunState::UWallrunState()
{
}

void UWallrunState::InitializeState(UCBCharacterMovementComponent* moveComponent)
{
	UBaseMovementState::InitializeState(moveComponent);

	DefaultGravityScale = MovementComponent->GravityScale;

}

void UWallrunState::Activate(ECBMovementMode previousMode)
{
	UBaseMovementState::Activate(previousMode);

	MovementComponent->GravityScale = 0.f;
	MovementComponent->Velocity.Z = 0;

	float length;
	MovementComponent->Velocity.ToDirectionAndLength(WallrunDirection, length);
	
	currInitializeAnimationFrames = 0;
}

void UWallrunState::Deactivate()
{
	UBaseMovementState::Deactivate();

	MovementComponent->GravityScale = DefaultGravityScale;

	MovementComponent->animinstance->SetIsWallRidingClockWise(false);
	MovementComponent->animinstance->SetIsWallRidingCounterClockWise(false);

	timeOnWall = 0.f;
}

void UWallrunState::OnTick(float DeltaTime)
{
	UBaseMovementState::OnTick(DeltaTime);

	float length;
	MovementComponent->Velocity.ToDirectionAndLength(WallrunDirection, length);
	WallrunDirection.Z = 0.f;

	if(bIsLaunching)
	{
		MovementComponent->Velocity = LaunchVector;
	}
	else
	{
		MovementComponent->Velocity = WallrunDirection * (MovementComponent->MaxAcceleration * MovementComponent->WallrunSpeedModifier);
		timeOnWall += DeltaTime;
	}
	
	//More then one Tick is needed to finalize the WallRunDirection, therefore the first 5 Ticks are used to correct the Animation.
	if (currInitializeAnimationFrames < initializeAnimationFrames)
	{
		FHitResult wallRunHitResult;
		EWallRunDirection hitPosition = HitDirection(wallRunHitResult);

		if (hitPosition == EWallRunDirection::WALLRUN_CLOCKWISE)
		{
			MovementComponent->animinstance->SetIsWallRidingClockWise(true);
			MovementComponent->animinstance->SetIsWallRidingCounterClockWise(false);
		}

		else
		{
			MovementComponent->animinstance->SetIsWallRidingCounterClockWise(true);
			MovementComponent->animinstance->SetIsWallRidingClockWise(false);
		}
		currInitializeAnimationFrames++;
	}
	else if (currInitializeAnimationFrames == initializeAnimationFrames)
	{
		ACyberbowlCharacter* character = Cast<ACyberbowlCharacter>(MovementComponent->GetCharacterOwner());
		character->OnFinishedInitializingWallrun.Broadcast();
		currInitializeAnimationFrames++;
	}
	
	FRotator wallrunDirRotator = WallrunDirection.Rotation();
	MovementComponent->GetCharacterOwner()->SetActorRotation(FRotator(0.f, wallrunDirRotator.Yaw, 0.f));
}

FVector UWallrunState::GetLaunchVector()
{
	FHitResult wallRunHitResult;
	HitDirection(wallRunHitResult);
	FVector wallNormal = wallRunHitResult.Normal;

	//Calculate the angle between the normal pointing away from the wall and the current velocity
	float angleRadians = FMath::UnwindRadians(FMath::Atan2(wallNormal.Y, wallNormal.X) - FMath::Atan2(WallrunDirection.Y, WallrunDirection.X));
	float angle = FMath::RadiansToDegrees(angleRadians);
	angle = FMath::Clamp(angle, -MovementComponent->WallrunLaunchAngle, MovementComponent->WallrunLaunchAngle);

	FVector launchVec = WallrunDirection.RotateAngleAxis(angle, FVector::UpVector);

	launchVec *= MovementComponent->WallrunLaunchForce;
	launchVec.Z = MovementComponent->WallrunUpwardsLaunchForce;

	return launchVec;
}

void UWallrunState::BindInputActions()
{
	InputComponent->BindAction("WallrunJump", IE_Pressed, this, &UWallrunState::LaunchCharacter);
}

void UWallrunState::LaunchCharacter()
{
	if(MovementComponent->GetCBMovementMode() != ECBMovementMode::CBMOVE_Wallrun)
	{
		return;
	}
	
	LaunchVector = GetLaunchVector();
	bIsLaunching = true;
	MovementComponent->animinstance->SetIsDashing(true);
	MovementComponent->GetWorld()->GetTimerManager().SetTimer(LaunchTimerHandle, this, &UWallrunState::EndWallrun, MovementComponent->WallrunLaunchDuration);
}

EWallRunDirection UWallrunState::HitDirection(FHitResult& hitResult)
{
	auto world = MovementComponent->GetWorld();
	auto actorLoc = MovementComponent->GetActorLocation();
	auto end1 = actorLoc + 200.f * WallrunDirection.RotateAngleAxis(90, FVector(0, 0, 1));
	auto end2 = actorLoc + 200.f * WallrunDirection.RotateAngleAxis(-90, FVector(0, 0, 1));

	FHitResult hit1, hit2;
	world->LineTraceSingleByProfile(hit1, actorLoc, end1, "WallrunTrace");
	world->LineTraceSingleByProfile(hit2, actorLoc, end2, "WallrunTrace");
	//DrawDebugLine(world, MovementComponent->GetActorLocation(), end1, FColor::Red, false, 5, 0, 3.f);
	//DrawDebugLine(world, MovementComponent->GetActorLocation(), end2, FColor::Blue, false, 5, 0, 3.f);
	FVector hitVec;
	if (hit1.bBlockingHit)
	{
		hitResult = hit1;
		return EWallRunDirection::WALLRUN_COUNTERCLOCKWISE;//WallrunDirection.RotateAngleAxis(-90, FVector(0, 0, 1));
	}
	else
	{
		hitResult = hit2;
		return EWallRunDirection::WALLRUN_CLOCKWISE;//WallrunDirection.RotateAngleAxis(90, FVector(0, 0, 1));
	}
}

void UWallrunState::EndWallrun()
{
	bIsLaunching = false;
	LaunchVector = WallrunDirection = FVector::ZeroVector;
	MovementComponent->animinstance->SetIsDashing(false);
	MovementComponent->SetCBMovementMode(ECBMovementMode::CBMOVE_Jump);
}

