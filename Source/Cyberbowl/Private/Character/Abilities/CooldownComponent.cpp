// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Abilities/CooldownComponent.h"
#include "TimerManager.h"


// Sets default values for this component's properties
UCooldownComponent::UCooldownComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


void UCooldownComponent::StartCooldown(FString ability)
{
	if (ability == "Dash")
	{
		DashReady = false;
		GetWorld()->GetTimerManager().SetTimer(DashCooldownHandle, this, &UCooldownComponent::SetDashReady, TotalDashCooldown, false);

		DashCooldownStarted.Broadcast();
	}
	else if (ability == "Ult")
	{
		UltReady = false;
		GetWorld()->GetTimerManager().SetTimer(UltCooldownHandle, this, &UCooldownComponent::SetUltReady, TotalUltCooldown, false);
	
		UltCooldownStarted.Broadcast();
	}
}

// Called when the game starts
void UCooldownComponent::BeginPlay()
{
	Super::BeginPlay();

	character = Cast<ACyberbowlCharacter>(GetOwner());
	
}

void UCooldownComponent::SetDashReady()
{
	DashReady = true;
	DashCooldownFinished.Broadcast();
}

void UCooldownComponent::SetUltReady()
{
	UltReady = true;
	UltCooldownFinished.Broadcast();
}


// Called every frame
void UCooldownComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

