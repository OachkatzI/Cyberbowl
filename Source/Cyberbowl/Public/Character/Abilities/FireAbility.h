// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityBase.h"

#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Character/CyberbowlCharacter.h"
#include "Character/Abilities/Firewall.h"
#include "FireAbility.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Abilities), meta = (BlueprintSpawnableComponent))
class CYBERBOWL_API UFireAbility : public UAbilityBase
{
	
	GENERATED_BODY()
public:

	UFireAbility() = default;

	void ResetTargeting() override;

	UPROPERTY(EditAnywhere)
	float fireWallLifeTime;

	UPROPERTY(EditAnywhere)
	float TargetDistance;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AFirewall> FireWallClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector fireWallExtent = (FVector(3000, 50, 1000));

private:

	virtual void BeginPlay() override;
	
	virtual void Fire() override;
	
	virtual void Targeting() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UCameraComponent* camera;
	FVector fireWallPosition;
	bool bValidTarget;

	//UStaticMeshComponent* targetingComponent;
	ACyberbowlCharacter* character;


};