// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Cyberbowl/Public/Character/CyberbowlCharacter.h"
#include "CooldownComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDashCooldownStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDashCooldownFinished);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUltCooldownStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUltCooldownFinished);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CYBERBOWL_API UCooldownComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCooldownComponent();

	UPROPERTY(BlueprintAssignable, category = "EventDispatchers")
	FOnDashCooldownStarted DashCooldownStarted;

	UPROPERTY(BlueprintAssignable, category = "EventDispatchers")
	FOnDashCooldownFinished DashCooldownFinished;

	UPROPERTY(BlueprintAssignable, category = "EventDispatchers")
	FOnUltCooldownStarted UltCooldownStarted;

	UPROPERTY(BlueprintAssignable, category = "EventDispatchers")
	FOnUltCooldownFinished UltCooldownFinished;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = "Cooldowns")
	float TotalDashCooldown;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = "Cooldowns")
	float TotalUltCooldown;

	UFUNCTION(BlueprintCallable, Category = "CooldownComponent")
	bool IsDashReady() { return DashReady; }
	
	UFUNCTION(BlueprintCallable, Category = "CooldownComponent")
	bool IsUltReady() { return UltReady; }
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION()
	void SetDashReady();

	UFUNCTION()
	void SetUltReady();

	UPROPERTY(BlueprintReadOnly)
	FTimerHandle DashCooldownHandle;

	UPROPERTY(BlueprintReadOnly)
	FTimerHandle UltCooldownHandle;

	UPROPERTY(BlueprintReadOnly)
	bool DashReady = true;

	UPROPERTY(BlueprintReadOnly)
	bool UltReady = true;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void StartCooldown(FString ability);

	ACyberbowlCharacter* character;
};
