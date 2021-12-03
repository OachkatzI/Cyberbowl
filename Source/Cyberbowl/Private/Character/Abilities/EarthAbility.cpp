// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Abilities/EarthAbility.h"

#include "Character/Abilities/AbilityUtils.h"
#include "Character/CyberbowlCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Character/Abilities/CooldownComponent.h"
#include "PlayerController/ThirdPersonPlayerController.h"

#include "Character/Abilities/CooldownComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Engine/Engine.h"
#include "Components/PrimitiveComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PlayerController/TutorialPlayerController.h"
#include "TimerManager.h"

void UEarthAbility::BeginPlay()
{
	Super::BeginPlay();

	character = Cast<ACyberbowlCharacter>(GetOwner());
	bTargetingVisible = false;
}

void UEarthAbility::Fire()
{
	
	if(!bValidTarget)
	{
		SetAbilityState(EAbilityState::ABILITY_DEFAULT);
		return;
	}
	
	ResetTargeting();
	SpawnPillar();
	
	auto cooldownComponent = GetOwner()->FindComponentByClass<UCooldownComponent>();
	cooldownComponent->StartCooldown("Ult");
	SetAbilityState(EAbilityState::ABILITY_COOLDOWN);
	bValidTarget = false;
	bTargetingVisible = false;
}

void UEarthAbility::Targeting()
{
	UWorld* world = GetWorld();
	auto ownerAsPawn = Cast<APawn>(GetOwner());
	FVector actorLoc = GetOwner()->GetActorLocation();
	//FVector camViewDir = ownerAsPawn->GetControlRotation().Vector();

	FRotator camViewRotator = ownerAsPawn->GetControlRotation();
	//Set Pitch to zero cause we are only interested in the Yaw (Roll is always 0)
	camViewRotator.Pitch = 0.f;
	FVector end = actorLoc + camViewRotator.Vector() * TargetDistance;
	FRotator indicatorRotation = FRotator(0.f, camViewRotator.Yaw, 0.f);

	bValidTarget = UAbilityUtils::FindTargetPoint(world, PillarSpawnPoint, actorLoc, end, TargetIndicatorRadius);

	if (bValidTarget)
	{
		if (!bTargetingVisible)
		{
			spawnedIndicator = GetWorld()->SpawnActor<AActor>(TargetingIndicator);
			spawnedIndicator->SetOwner(character);
			bTargetingVisible = true;
		}

		spawnedIndicator->SetActorLocation(PillarSpawnPoint);
		spawnedIndicator->SetActorRotation(indicatorRotation);
		//targetingComponent->SetWorldLocation(FVector(PillarSpawnPoint.X, PillarSpawnPoint.Y, PillarSpawnPoint.Z+50.f));
	}

	else
	{
		ResetTargeting();
	}
#pragma region Dynamic Targeting (old)
	//bValidTarget = true;
	//if (hitResult.bBlockingHit)
	//{
	//	LeapTarget = hitResult.ImpactPoint;

	//	//Edge Case when player aims at the Wall
	//	if(hitResult.GetComponent()->GetCollisionProfileName() == "StadiumWall")
	//	{
	//		//get a point a certain distant away from the wall
	//		FVector hitPointWithOffset = hitResult.ImpactPoint + hitResult.ImpactNormal * TargetIndicatorRadius;

	//		FHitResult floorTrace;
	//		world->LineTraceSingleByProfile(floorTrace, hitPointWithOffset, hitPointWithOffset + FVector::DownVector * 10000.f, "AbilityTrace");

	//		if (floorTrace.bBlockingHit)
	//		{
	//			float distanceToActor = (floorTrace.ImpactPoint - actorLoc).Size();
	//			if (distanceToActor < MinTargetDistance)
	//			{
	//				bValidTarget = false;
	//			}
	//			else
	//			{
	//				LeapTarget = floorTrace.ImpactPoint;
	//			}
	//		}
	//	}
	//	//case if the player aims at the ground close to character
	//	else if(hitResult.Distance < MinTargetDistance)
	//	{
	//		LeapTarget = FVector(actorLoc.X, actorLoc.Y, hitResult.ImpactPoint.Z) + camViewRotator.Vector() * MinTargetDistance;
	//	}
	//}
	//else
	//{
	//	FHitResult floorTrace;
	//	world->LineTraceSingleByProfile(floorTrace, end, end + FVector::DownVector * 10000.f, "AbilityTrace");

	//	if(floorTrace.bBlockingHit)
	//	{
	//		//This check is needed when player aims straight into the air
	//		float distanceToActor = (floorTrace.ImpactPoint - actorLoc).Size();
	//		if(distanceToActor < MinTargetDistance)
	//		{
	//			LeapTarget = FVector(actorLoc.X, actorLoc.Y, hitResult.ImpactPoint.Z) + camViewRotator.Vector() * MinTargetDistance;
	//		}
	//		else
	//		{
	//			LeapTarget = floorTrace.ImpactPoint;
	//		}
	//	}
	//}
#pragma endregion 
}

void UEarthAbility::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
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

	if (!characterController)
	{
		characterController = Cast<AThirdPersonPlayerController>(character->GetController());
	}
}

void UEarthAbility::SpawnPillar()
{
	auto ownerAsPawn = Cast<APawn>(GetOwner());
	FRotator camViewRotator = ownerAsPawn->GetControlRotation();
	FRotator indicatorRotation = FRotator(0.f, camViewRotator.Yaw, 0.f);
	auto pillar = GetWorld()->SpawnActor<AEarthpillar>(EarthPillarClass, PillarSpawnPoint, indicatorRotation);

	if (!characterController)
	{
		pillar->InitializePillar(1, PillarSpawnPoint.Z, PillarLifeSpan);
	}
	else
	{
		pillar->InitializePillar(characterController->currPlayerTeam, PillarSpawnPoint.Z, PillarLifeSpan);
	}
	pillar->OnActorLaunched.AddDynamic(this, &UEarthAbility::CallOnActorLaunched);
	//pillar->SetActorLocation(pillar->GetActorLocation() - FVector(0, pillar->GetPillarLocationZ(), 0));
}

void UEarthAbility::CallOnActorLaunched(AActor* launchedActor)
{
	OnAActorLaunched.Broadcast(launchedActor);
}

void UEarthAbility::ResetTargeting()
{
	Super::ResetTargeting();
	bValidTarget = false;
}
