// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BoopComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Actors/PlayBall.h"
#include "Components/BoxComponent.h"
#include "Character/CyberbowlCharacterAnimInstance.h"
#include "Engine/Engine.h"
#include "NiagaraFunctionLibrary.h"
#include "TimerManager.h"
#include "GameFramework/SpringArmComponent.h"




// Sets default values for this component's properties
UBoopComponent::UBoopComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UBoopComponent::BeginPlay()
{
	Super::BeginPlay();

	MovementComponent = GetOwner()->FindComponentByClass<UCBCharacterMovementComponent>();

	BoopSpringarm = Cast<USpringArmComponent>(GetOwner()->GetComponentsByTag(USpringArmComponent::StaticClass(), "BoopSpringarm").Last());
	BoopEffectSpawnLocation = Cast<USceneComponent>(GetOwner()->GetComponentsByTag(USceneComponent::StaticClass(), "BoopEffectSpawnLocation").Last());
	BoopHitbox = Cast<UBoxComponent>(GetOwner()->GetComponentsByTag(UBoxComponent::StaticClass(), "BoopHitbox").Last());

	if (!BoopHitbox || !BoopEffectSpawnLocation || !BoopSpringarm)
	{
		UE_LOG(LogInit, Error, TEXT("BoopComponent: Vital Components are not set in Character Blueprint! (BoopSpringarm, BoopEffectSpawnLocation, BoopHitbox)"));
	}
	else
	{
		BoopHitbox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		BoopHitbox->OnComponentBeginOverlap.AddDynamic(this, &UBoopComponent::PushBall);
		BoopHitboxInitialLocation = BoopHitbox->GetRelativeLocation();
		BoopHitbox->SetAbsolute(false, true, false);
	}
}

void UBoopComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorld()->GetTimerManager().ClearTimer(BoopDurationHandle);
}

void UBoopComponent::StartBoop()
{
	auto owner = GetOwner();
	if(owner->GetWorld()->GetTimerManager().IsTimerActive(BoopCooldownHandle))
	{
		return;
	}
	
	OnBoopStarted.Broadcast();

	//Set Cooldown
	owner->GetWorld()->GetTimerManager().SetTimer(BoopCooldownHandle, this, &UBoopComponent::OnBoopCooldown, BoopCooldown);
	

	if (!bBoopActive)
	{
		owner->GetWorld()->GetTimerManager().SetTimer(BoopDurationHandle, this, &UBoopComponent::EndBoop, BoopDuration);

		//TODO: The animation is 1 second long with exactly 30 frames... this is hardcoded for now for convenience
		auto animPlayRate = 1.f / 30.f * (30.f / BoopDuration);
		
		bBoopActive = true;
		BoopHitbox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		MovementComponent->animinstance->SetIsBooping(true);
		MovementComponent->animinstance->SetBoopPlayRate(animPlayRate);
	}

}

void UBoopComponent::EndBoop()
{
	DeactivateBoopHitbox();
	bBoopActive = false;
	MovementComponent->animinstance->SetIsBooping(false);

	//Set the Pitch of the actor to 0 so his orientation is back to normal
	auto ownerAsPawn = Cast<APawn>(GetOwner());
	auto cameraRot = ownerAsPawn->GetControlRotation();
	cameraRot.Pitch = 0.f;
	GetOwner()->SetActorRotation(cameraRot);

	
	if (BoopNiagaraComponent)
	{
		BoopNiagaraComponent->DestroyComponent();
		BoopNiagaraComponent = nullptr;
	}

}

void UBoopComponent::PushBall(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto ball = Cast<APlayBall>(OtherActor);
	if(!ball)
	{
		return;
	}
	
	auto ownerAsPawn = Cast<APawn>(GetOwner());
	FVector cameraForwardVec = ownerAsPawn->GetControlRotation().Vector();

	if (MovementComponent->GetCBMovementMode() == ECBMovementMode::CBMOVE_Running)
	{
		cameraForwardVec.Z = FMath::DegreesToRadians(UpwardsAngle);
		//DrawDebugLine(Owner->GetWorld(), Owner->GetActorLocation(), Owner->GetActorLocation() + 100.f * cameraForwardVec, FColor::Emerald, false, 10.f, 0, 5.f);
	}

	//extra linetrace to determine the distance of ball to cast center
	//the push force is lowered the higher the distance
	FHitResult traceToBall;
	GetWorld()->LineTraceSingleByProfile(traceToBall, BoopEffectSpawnLocation->GetComponentLocation(), OtherActor->GetActorLocation(), "BlockAll");

	auto pushForce = Force;
	if(traceToBall.bBlockingHit)
	{
		auto hitboxExtent = BoopHitbox->GetScaledBoxExtent();
		auto distanceNormalized = traceToBall.Distance / (hitboxExtent.X * 2.f);

		auto forceModifier = ForceFalloffCurve->GetFloatValue(FMath::Clamp(distanceNormalized, 0.01f, 1.f));
		
		pushForce *= forceModifier;
	}

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Emerald, FString::Printf(TEXT("extent: %f, %f, %f"), hitDistance.X, hitDistance.Y, hitDistance.Z));
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Emerald, FString::Printf(TEXT("force: %f"), pushForce));
	
	ball->PushBall(GetOwner(), pushForce, cameraForwardVec);

	//deactivate hitbox so ball only gets pushed once when inside hitbox
	DeactivateBoopHitbox();
}

void UBoopComponent::DeactivateBoopHitbox()
{
	if(bBoopActive)
	{
		//TODO: fire event
		BoopHitbox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void UBoopComponent::SpawnBoopEffect()
{
	if(bBoopActive)
	{
		//effect needs to be rotated 90 degrees since it is not oriented the right way
		FRotator spawnRot = FRotator(-90, 0, 0) + MovementComponent->GetPawnOwner()->GetControlRotation();
		
		BoopNiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(BoopEffect, GetOwner()->GetRootComponent(), NAME_None, BoopEffectSpawnLocation->GetComponentLocation(), spawnRot, EAttachLocation::KeepWorldPosition, false);
	}
}

void UBoopComponent::AdjustBoopTransforms(float DeltaTime)
{
	if(!bBoopActive)
	{
		return;
	}
	auto ownerAsPawn = Cast<APawn>(GetOwner());
	FVector cameraForwardVec = ownerAsPawn->GetControlRotation().Vector();

	BoopHitbox->SetWorldRotation(cameraForwardVec.Rotation());
	BoopSpringarm->SetWorldRotation(cameraForwardVec.Rotation());

	//TODO: we cant use the Pitch during the rotation, since it fks up the camera... especially when resetting the rotation on EndBoop
	auto cameraRot = cameraForwardVec.Rotation();
	cameraRot.Pitch = 0.f;
	GetOwner()->SetActorRotation(cameraRot);
	//DEBUG
	//if (bBoopActive)
	//	DrawDebugBox(GetOwner()->GetWorld(), BoopHitbox->GetComponentLocation(), BoopHitbox->GetScaledBoxExtent(), BoopHitbox->GetComponentRotation().Quaternion(), FColor::Red, false, DeltaTime, 0, 3.f);
}

void UBoopComponent::OnBoopCooldown()
{
	//TODO: fire event
}

// Called every frame
void UBoopComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	//these assignments do not work in BeginPlay(), because playercontroller is assigned afterwards at some point
	if(!PlayerController)
	{
		auto pawn = Cast<APawn>(GetOwner());
		if (!pawn)
		{
			return;
		}

		auto pc = Cast<APlayerController>(pawn->GetController());
		PlayerController = pc;
	}

	AdjustBoopTransforms(DeltaTime);
}

