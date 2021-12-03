// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityBase.h"
#include "Character/Abilities/Earthpillar.h"
#include "EarthAbility.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAActorLaunched, AActor*, launchedActor);

UCLASS(ClassGroup = (Abilities), meta = (BlueprintSpawnableComponent))
class CYBERBOWL_API UEarthAbility : public UAbilityBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, category = "EventDispatchers")
	FOnAActorLaunched OnAActorLaunched;
	
protected:

	void BeginPlay() override;
	
	void Fire() override;

	void Targeting() override;
	
	void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void ResetTargeting() override;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//float MinTargetDistance = 500.f;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//float MaxTargetDistance = 3000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TargetDistance = 5000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TargetIndicatorRadius = 400.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PillarLifeSpan = 4.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AEarthpillar> EarthPillarClass;
	//UPROPERTY(BlueprintReadOnly)
	//FVector LeapTarget;

	//UPROPERTY(BlueprintReadOnly)
	//FVector LeapStart;

	UPROPERTY(BlueprintReadOnly)
	FVector PillarSpawnPoint;

	UPROPERTY()
	bool bValidTarget;

	class AThirdPersonPlayerController* characterController;

	void SpawnPillar();

	class ACyberbowlCharacter* character;

	UFUNCTION()
	void CallOnActorLaunched(AActor* launchedActor);
};


