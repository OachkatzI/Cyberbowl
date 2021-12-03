// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/Abilities/FireAbility.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "Camera/PlayerCameraManager.h"
#include "Camera/CameraComponent.h"
#include "Character/Abilities/AbilityUtils.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Character/Abilities/CooldownComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"

void UFireAbility::BeginPlay()
{
	Super::BeginPlay();
	fireWallPosition = FVector(0.f);
	bValidTarget = false;
	bTargetingVisible = false;
	character = Cast<ACyberbowlCharacter>(GetOwner());
}


void UFireAbility::Fire()
{

	if(!bValidTarget)
	{
		SetAbilityState(EAbilityState::ABILITY_DEFAULT);
		return;
	}

	ResetTargeting();
	
	FVector boxPosition = fireWallPosition + FVector(0.f, 0.f, fireWallExtent.Z);
	auto rotation = character->GetCameraBoom()->GetTargetRotation();
	FRotator indicatorRotation = FRotator(0.f, rotation.Yaw + 90, 0.f);
	

	AFirewall* firewall = GetWorld()->SpawnActorDeferred<AFirewall>(FireWallClass, FTransform(indicatorRotation.Quaternion(), boxPosition));

	firewall->SetLifeSpan(fireWallLifeTime);
	firewall->SetBoxExtent(fireWallExtent);
	UGameplayStatics::FinishSpawningActor(firewall, firewall->GetTransform());

	auto cooldownComponent = character->FindComponentByClass<UCooldownComponent>();
	cooldownComponent->StartCooldown("Ult");
	SetAbilityState(EAbilityState::ABILITY_COOLDOWN);
	
}

void UFireAbility::Targeting()
{
	UWorld* world = GetWorld();

	FVector actorLoc = character->GetActorLocation();
	auto actorRot = character->GetCameraBoom()->GetTargetRotation();
	//Set Pitch to zero cause we are only interested in the Yaw (Roll is always 0)
	actorRot.Pitch = 0.f;
	FRotator indicatorRotation = FRotator(0.f, actorRot.Yaw + 90, 0.f);
	FVector end = actorLoc + actorRot.Vector() * TargetDistance;

	bValidTarget = UAbilityUtils::FindTargetPoint(world, fireWallPosition, actorLoc, end, 100.f);

	if (bValidTarget)
	{
		if (!bTargetingVisible)
		{
			spawnedIndicator = GetWorld()->SpawnActor<AActor>(TargetingIndicator, fireWallPosition, indicatorRotation);
			spawnedIndicator->SetActorScale3D(FVector(fireWallExtent) / 50.f);
			spawnedIndicator->SetOwner(character);
			bTargetingVisible = true;
		}

		spawnedIndicator->SetActorLocation(fireWallPosition + FVector(0.f, 0.f, fireWallExtent.Z));
		spawnedIndicator->SetActorRotation(indicatorRotation);
	}

	else
	{
		ResetTargeting();
	}
}

void UFireAbility::ResetTargeting()
{
	Super::ResetTargeting();
	bValidTarget = false;
}

void UFireAbility::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (CurrState == EAbilityState::ABILITY_FIRE)
	{
		Fire();
	}
	else if (CurrState == EAbilityState::ABILITY_TARGETING)
	{
		Targeting();
	}
}