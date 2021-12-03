// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Abilities/AirAbility.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameModesAndInstances/InGameGameMode.h"
#include "DrawDebugHelpers.h"
#include "Actors/PlayBall.h"
#include "Character/CyberbowlCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Actors/PlayBall.h"
#include "PlayerController/ThirdPersonPlayerController.h"
#include "Character/BallCamComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/SpringArmComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Character/Abilities/CooldownComponent.h"
#include "Character/CyberbowlCharacterAnimInstance.h"

void UAirAbility::BeginPlay()
{
	Super::BeginPlay();
	
	character = Cast<ACyberbowlCharacter>(GetOwner());
	movementComp = Cast<UCBCharacterMovementComponent>(character->GetMovementComponent());
	bTargetingVisible = false;
	
	ball = Cast<APlayBall>(UGameplayStatics::GetActorOfClass(this, APlayBall::StaticClass()));
	ballLocationSpringArm = Cast<USpringArmComponent>(character->GetComponentsByTag(USpringArmComponent::StaticClass(), "BallLocationArm").Last());
	ballPulledAttachComponent = Cast<USceneComponent>(character->GetComponentsByTag(USceneComponent::StaticClass(), "TornadoBallLocation").Last());

	ball->OnBallBooped.AddDynamic(this, &UAirAbility::ExitGrabModeByPush);


}

void UAirAbility::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(CurrState == EAbilityState::ABILITY_FIRE)
	{
		Fire();
	}
	else if(CurrState == EAbilityState::ABILITY_TARGETING)
	{
		Targeting();
	}

	if (bIsInGrabMode)
	{		
		
		if (UKismetMathLibrary::Abs(FVector::Distance(ball->GetActorLocation(), ballPulledAttachComponent->GetComponentLocation())) <= 100.f)
		{
			ball->AttachToComponent(ballPulledAttachComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		}
		else
		{
			const auto ballTarget = UKismetMathLibrary::VInterpTo(ball->GetActorLocation(), ballPulledAttachComponent->GetComponentLocation(), DeltaTime, succSpeed);
			ball->SetActorLocation(ballTarget);
		}

		const FRotator cameraLookAt = FRotator(character->GetCameraBoom()->GetTargetRotation().Pitch * (-1), ballLocationSpringArm->GetComponentRotation().Yaw, 0);
		ballLocationSpringArm->SetWorldRotation(cameraLookAt);

		if(!movementComp->animinstance->GetIsGrabbing())
		{
			movementComp->animinstance->SetIsGrabbing(true);
		}
	}
}

void UAirAbility::Fire()
{
	ResetTargeting();
	FVector cylinderEnd = GetOwner()->GetActorLocation();
	cylinderEnd.Z += 2000.f;
	//DrawDebugCylinder(GetWorld(), GetOwner()->GetActorLocation(), cylinderEnd, grabRadiusMeters, 32, FColor::Red, false, grabDurationSeconds, 0, 5.f);

	const float distance = FVector2D::Distance(FVector2D(GetOwner()->GetActorLocation().X, GetOwner()->GetActorLocation().Y),FVector2D(ball->GetActorLocation().X, ball->GetActorLocation().Y));
	
	if (distance <= grabRadiusMeters)
	{
		bIsInGrabMode = true;

		// Used for character control with right stick
		character->bTurretMode = true;
		
		//ball->StopBall();
		ball->BallStaticMesh->SetEnableGravity(false);
		movementComp->DisableMovement();
		
		// Disable BallCam if active
		auto ballCamComp = Cast<UBallCamComponent>(Cast<ACyberbowlCharacter>(GetOwner())->GetComponentByClass(UBallCamComponent::StaticClass()));
		ballCamComp->DoNotFollow();

		GetWorld()->GetTimerManager().SetTimer(GrabModeDurationHandle, this, &UAirAbility::ExitGrabMode, grabDurationSeconds, false);

		const auto cameraLookAtRotation = FRotator(0.f, character->GetCameraBoom()->GetTargetRotation().Yaw, 0.f);
		character->SetActorRotation(cameraLookAtRotation);

		OnGrabMode.Broadcast();
	}

	else
	{
		OnFailedGrab.Broadcast();
	}

	tornadoComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, tornadoEffect, character->GetActorLocation());
	GetWorld()->GetTimerManager().SetTimer(TornadoEffectDurationHandle, this, &UAirAbility::DestroyTornado, tornadoDuration, false);

	auto cooldownComponent = character->FindComponentByClass<UCooldownComponent>();
	cooldownComponent->StartCooldown("Ult");
	SetAbilityState(EAbilityState::ABILITY_COOLDOWN);
	playSoundTargeting = true;
	bTargetingVisible = false;
}

void UAirAbility::Targeting()
{
	
	FVector cylinderEnd = GetOwner()->GetActorLocation();
	cylinderEnd.Z += 2000.f;
	FVector cylinderPosition = FVector(character->GetActorLocation().X, character->GetActorLocation().Y, (cylinderEnd.Z) / 2);
	if (!bTargetingVisible)
	{
		spawnedIndicator = GetWorld()->SpawnActor<AActor>(TargetingIndicator, cylinderPosition, FRotator::ZeroRotator);

		spawnedIndicator->SetActorScale3D(FVector(grabRadiusMeters/50, grabRadiusMeters/50, cylinderEnd.Z/100));

		spawnedIndicator->SetOwner(character);

		//targetingComponent->SetVisibility(true);
		bTargetingVisible = true;
	}
	
	spawnedIndicator->SetActorLocation(cylinderPosition);
	if (playSoundTargeting)
	{
		OnTargeting.Broadcast();
		playSoundTargeting = false;
	}
}

void UAirAbility::ConvertMetersToUnrealUnits()
{
	// Conversion from meters to cm, as unreal functions generally output centimeters, but meters is easier for game design tweaks
	grabRadiusMeters *= 100.f;
}

void UAirAbility::ExitGrabModeByPush(AActor* instigator, float force)
{
	if (!bIsInGrabMode)
	{
		return;
	}
	
	ExitGrabMode();
	OnGrabModeExitByPush.Broadcast();

}

void UAirAbility::ExitGrabMode()
{
	if (!bIsInGrabMode)
	{
		return;
	}
	
	bIsInGrabMode = false;
	ball->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	ball->BallStaticMesh->SetEnableGravity(true);

	character->bTurretMode = false;
	character->GetCameraBoom()->bUsePawnControlRotation = true;
	movementComp->SetMovementMode(EMovementMode::MOVE_Walking);

	movementComp->animinstance->SetIsGrabbing(false);
	OnGrabModeEnd.Broadcast();
}

void UAirAbility::DestroyTornado()
{

	tornadoComponent->DestroyComponent();
	
}

void UAirAbility::ResetTargeting()
{
	Super::ResetTargeting();

}