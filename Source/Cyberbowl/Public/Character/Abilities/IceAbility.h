// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityBase.h"
#include "IceAbility.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAbilityCasted);

UCLASS(ClassGroup = (Abilities), meta = (BlueprintSpawnableComponent))
class CYBERBOWL_API UIceAbility : public UAbilityBase
{
	GENERATED_BODY()
public:
	virtual void Fire() override;

	void Targeting() override;

	void ResetTargeting() override;

	//UFUNCTION(BlueprintCallable)
	//bool IsBallFrozen()
	//{
	//	return bIsBallFrozen;
	//}
protected:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IceAbilityParams")
	float ConeLength = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IceAbilityParams")
	float ConeAngle = 45.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IceAbilityParams")
	float FreezeDuration = 3.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IceAbilityParams")
	class UNiagaraSystem* CoCEffect;

	UPROPERTY(BlueprintReadOnly, Category = "IceAbility")
	class UNiagaraComponent* NiagaraComponent;

	UPROPERTY(BlueprintAssignable, Category = "IceAbility")
	FOnAbilityCasted OnAbilityCasted;
	
	UPROPERTY()
	FTimerHandle FreezeTimerHandle;

	UPROPERTY()
	FTimerHandle CoCEffectDurationHandle;

	UPROPERTY()
	TArray<AActor*> FrozenActors;

	//UPROPERTY()
	//bool bIsBallFrozen;
	//UFUNCTION()
	//void SetBallFrozen() { bIsBallFrozen = true; }
	//UFUNCTION()
	//void SetBallUnfrozen() { bIsBallFrozen = false; }
	
	UFUNCTION(BlueprintCallable, Category = "IceAbility")
	void UnfreezeActors();
	
	bool IsWithinCone(FVector hitPoint, FVector coneDirectionNormal);

	void DestroyCoCEffect();
};
