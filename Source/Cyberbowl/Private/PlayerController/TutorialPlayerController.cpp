// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerController/TutorialPlayerController.h"

#include "Character/CyberbowlCharacter.h"
#include "Components/WidgetComponent.h"

ACyberbowlCharacter* ATutorialPlayerController::SwitchCharacterClass(TSubclassOf<ACyberbowlCharacter> newCharacterClass)
{
	if (currentHud)
	{
		currentHud->RemoveFromParent();
	}
	
	auto newCharacter = Cast<ACyberbowlCharacter>(GetWorld()->SpawnActor<AActor>(newCharacterClass, GetCharacter()->GetTransform()));

	GetWorld()->DestroyActor(GetCharacter());

	Possess(newCharacter);

	const auto cooldownComponent = Cast<UCooldownComponent>(newCharacter->GetComponentByClass(UCooldownComponent::StaticClass()));
	cooldownComponent->TotalUltCooldown = 2.f;

	TArray<UActorComponent*> widgetComponents;
	Cast<ACyberbowlCharacter>(GetCharacter())->GetComponents(UWidgetComponent::StaticClass(), widgetComponents);

	for (auto widget : widgetComponents)
	{
		widget->DestroyComponent();
	}

	const auto characerClassName = newCharacterClass->GetName();
	
	if (characerClassName.Contains("fire"))
	{
		currPlayerType = ECBCharacterType::CBCHRACTERTYPE_FIRE;
	}
	else if (characerClassName.Contains("ice"))
	{
		currPlayerType = ECBCharacterType::CBCHRACTERTYPE_ICE;
	}
	else if (characerClassName.Contains("air"))
	{
		currPlayerType = ECBCharacterType::CBCHRACTERTYPE_AIR;
	}
	else if (characerClassName.Contains("earth"))
	{
		currPlayerType = ECBCharacterType::CBCHRACTERTYPE_EARTH;
	}
	
	currentHud = CreateWidget<UUserWidget>(this, baseHudClass);
	currentHud->AddToPlayerScreen();
	
	return newCharacter;
}

void ATutorialPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("AdvanceTutorial", IE_Pressed, this, &ATutorialPlayerController::CallOnAdvanceTutorial);
}

void ATutorialPlayerController::CallOnAdvanceTutorial()
{
	OnAdvanceTutorial.Broadcast();
}

void ATutorialPlayerController::BeginPlay()
{
	Super::BeginPlay();

	SwitchCharacterClass(defaultCharacterClass);
}
