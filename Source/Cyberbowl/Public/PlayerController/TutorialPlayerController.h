// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Character/CBCharacterMovementComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TutorialPlayerController.generated.h"

class ACyberbowlCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAdvanceTutorial);

UCLASS()
class CYBERBOWL_API ATutorialPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, category = "EventDispatchers")
	FOnAdvanceTutorial OnAdvanceTutorial;

	ACyberbowlCharacter* SwitchCharacterClass(TSubclassOf<ACyberbowlCharacter> newCharacterClass);

	UPROPERTY(BlueprintReadOnly)
	ECBCharacterType currPlayerType;
	
protected:
	void SetupInputComponent() override;

	UFUNCTION()
	void CallOnAdvanceTutorial();
	void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ACyberbowlCharacter> defaultCharacterClass;
	
	UPROPERTY(Editanywhere)
	TSubclassOf<UUserWidget> baseHudClass;

	UPROPERTY()
	UUserWidget* currentHud;
};
