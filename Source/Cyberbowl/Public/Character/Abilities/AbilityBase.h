// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AbilityBase.generated.h"

//UENUM(BlueprintType)
enum class EAbilityState : uint8
{
	ABILITY_TARGETING,
	ABILITY_FIRE,
	ABILITY_COOLDOWN,
	ABILITY_DEFAULT
};

UCLASS( ClassGroup=(Abilities), meta=(BlueprintSpawnableComponent) )
class CYBERBOWL_API UAbilityBase : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAbilityBase();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	virtual void Fire();

	UFUNCTION(BlueprintCallable)
	virtual void Targeting();



	UFUNCTION(BlueprintCallable)
	void ResetAbilityState();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting")
	TSubclassOf<AActor> TargetingIndicator;

	AActor* spawnedIndicator;
	bool bTargetingVisible;
	
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	EAbilityState CurrState;
	virtual EAbilityState GetAbilityState() { return CurrState; };
	virtual void SetAbilityState(EAbilityState state);
	virtual void ResetTargeting();
};
