// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Abilities/IceAbility.h"

#include "Actors/IFreezeable.h"
#include "Actors/PlayBall.h"
#include "Character/Abilities/CooldownComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "DrawDebugHelpers.h"
#include "Engine/Engine.h"
#include "TimerManager.h"
#include "GameFramework/PlayerController.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

void UIceAbility::Fire()
{
	ResetTargeting();
	OnAbilityCasted.Broadcast();
	float coneAngleInRadians = FMath::DegreesToRadians(ConeAngle);
	float coneRadius = ConeLength * FMath::Tan(ConeAngle) / 2.f;
	APawn* ownerAsPawn = Cast<APawn>(GetOwner());

	FCollisionShape shape = FCollisionShape::MakeSphere(coneRadius);
	FVector start = GetOwner()->GetActorLocation();
	FVector direction = ownerAsPawn->GetControlRotation().Vector();
	FVector end = start + direction * ConeLength;
	TArray<FHitResult> hitResults;

	//DEBUG: Draw Cone Shape
	//DrawDebugCone(GetOwner()->GetWorld(), start, direction, ConeLength, coneAngleInRadians, coneAngleInRadians, 12, FColor::Blue, false, FreezeDuration, 0, 4.f);
	//DrawDebugSphere(GetOwner()->GetWorld(), GetOwner()->GetActorLocation(), coneRadius, 12, FColor::Emerald, false, 4, 0, 2.f);
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Emerald, FString::Printf(TEXT("%f"), coneRadius));

	//Spawn Niagara Effect
	FRotator effectRotation = ownerAsPawn->GetControlRotation().Add(0.f, -45.f, 0.f);
	NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetOwner(), CoCEffect, start, effectRotation);
	//GetWorld()->GetTimerManager().SetTimer(CoCEffectDurationHandle, this, &UIceAbility::DestroyCoCEffect, 1.f);
	
	FCollisionQueryParams queryParams;
	queryParams.AddIgnoredActor(GetOwner());
	GetOwner()->GetWorld()->SweepMultiByProfile(hitResults, start + direction * coneRadius, end - direction * coneRadius, FQuat(), "OverlapDynamics", shape, queryParams);

	TArray<AActor*> hitActors;
	for(auto hit : hitResults)
	{
		if(!hitActors.Contains(hit.GetActor()) && IsWithinCone(hit.ImpactPoint, direction))
		{
			hitActors.Add(hit.GetActor());
			DrawDebugPoint(GetOwner()->GetWorld(), hit.ImpactPoint, 10.f, FColor::Black, false, 5.f);
		}
	}

	for(auto actor : hitActors)
	{
		if(actor->GetClass()->ImplementsInterface(UFreezeable::StaticClass()))
		{
			IFreezeable::Execute_Freeze(actor, GetOwner());
			FrozenActors.Add(actor);
		}
	}

	if(FrozenActors.Num() > 0)
	{
		GetWorld()->GetTimerManager().SetTimer(FreezeTimerHandle, this, &UIceAbility::UnfreezeActors, FreezeDuration);
	}

	auto cooldownComponent = GetOwner()->FindComponentByClass<UCooldownComponent>();
	cooldownComponent->StartCooldown("Ult");
	SetAbilityState(EAbilityState::ABILITY_COOLDOWN);
	bTargetingVisible = false;
}

void UIceAbility::Targeting()
{
	float coneAngleInRadians = FMath::DegreesToRadians(ConeAngle);
	APawn* ownerAsPawn = Cast<APawn>(GetOwner());

	FVector start = GetOwner()->GetActorLocation();
	FVector direction = ownerAsPawn->GetControlRotation().Vector();

	if (!bTargetingVisible)
	{
		float radius = ConeLength/70* tan(coneAngleInRadians);
		spawnedIndicator = GetWorld()->SpawnActor<AActor>(TargetingIndicator);
		spawnedIndicator->SetActorScale3D(FVector(radius, radius, ConeLength / 125));
		spawnedIndicator->SetOwner(ownerAsPawn);
		bTargetingVisible = true;
	}

	FRotator controlRotation = ownerAsPawn->GetControlRotation();
	spawnedIndicator->SetActorRotation(FRotator(controlRotation.Pitch + 90, controlRotation.Yaw , controlRotation.Roll));
	direction.Normalize(0);
	spawnedIndicator->SetActorLocation(start + direction * ConeLength/2.5);
}

void UIceAbility::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(CurrState == EAbilityState::ABILITY_TARGETING)
	{
		Targeting();
	}
	if(CurrState == EAbilityState::ABILITY_FIRE)
	{
		Fire();
	}
}

void UIceAbility::BeginPlay()
{
	Super::BeginPlay();

	ACyberbowlCharacter* character = Cast<ACyberbowlCharacter>(GetOwner());
	bTargetingVisible = false;
}

void UIceAbility::UnfreezeActors()
{
	for(auto actor : FrozenActors)
	{
		if (actor->GetClass()->ImplementsInterface(UFreezeable::StaticClass()))
		{
			IFreezeable::Execute_UnFreeze(actor);
		}
	}
	FrozenActors.Empty();
}

bool UIceAbility::IsWithinCone(FVector hitPoint, FVector coneDirectionNormal)
{
	FVector actorLoc = GetOwner()->GetActorLocation();
	FVector hitDirection = hitPoint - actorLoc;

	float angleRadians = FMath::Acos(FVector::DotProduct(coneDirectionNormal, hitDirection.GetSafeNormal()));
	float angleDegrees = FMath::RadiansToDegrees(angleRadians);

	FVector projectedHitPoint = hitDirection.ProjectOnToNormal(coneDirectionNormal);
	float length = projectedHitPoint.Size();
	
	return angleDegrees <= ConeAngle && length <= ConeLength;
}

void UIceAbility::DestroyCoCEffect()
{
	NiagaraComponent->DestroyComponent();
	NiagaraComponent = nullptr;
}

void UIceAbility::ResetTargeting()
{
	Super::ResetTargeting();
}