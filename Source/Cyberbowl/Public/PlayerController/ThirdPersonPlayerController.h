// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Character/CyberbowlCharacter.h"
#include "Blueprint/UserWidget.h"
#include "FPlayerInfo.h"
#include "Components/WidgetComponent.h"
#include "ThirdPersonPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCallGameOverMenuNavigated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerPausedGame, int, playerIndexInitiator);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMenuNavigatedDown);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMenuNavigatedUp);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMenuNavigatedRight);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMenuNavigatedLeft);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMenuEnter);

UCLASS()
class CYBERBOWL_API AThirdPersonPlayerController : public APlayerController
{
	GENERATED_BODY()


public:
	void BeginPlay() override;

	void Tick(float DeltaSeconds) override;

	UFUNCTION()
	void SetupNameTagWidgets();

private:
	void SetupInputComponent() override;

	UFUNCTION(BlueprintCallable)
	void SpawnActors();

	UFUNCTION()
	void OnStartGamePlay();

	UFUNCTION()
	void OnGoalScored();

	UFUNCTION()
	void OnRegroup();

	UFUNCTION()
	void OnEndGame();

	UPROPERTY(Editanywhere)
	TSubclassOf<ACyberbowlCharacter> fireClass;

	UPROPERTY(Editanywhere)
	TSubclassOf<ACyberbowlCharacter> earthClass;

	UPROPERTY(Editanywhere)
	TSubclassOf<ACyberbowlCharacter> iceClass;

	UPROPERTY(Editanywhere)
	TSubclassOf<ACyberbowlCharacter> airClass;

	UPROPERTY(Editanywhere)
	TSubclassOf<UUserWidget> baseHudClass;

	UPROPERTY(Editanywhere)
	UTexture2D* arrowIndicatorTeamRed;

	UPROPERTY(Editanywhere)
	UTexture2D* arrowIndicatorTeamBlue;

	UFUNCTION()
	void CallGameOverMenuNavigated();

	UFUNCTION()
	void CallPlayerPausedGame();

	UFUNCTION()
	void CallMenuNavigationDown();

	UFUNCTION()
	void CallMenuNavigationUp();

	UFUNCTION()
	void CallMenuNavigationLeft();

	UFUNCTION()
	void CallMenuNavigationRight();

	void UpdateNametagPositions();
	

public:
	UPROPERTY(BlueprintAssignable, category = "EventDispatchers")
	FOnCallGameOverMenuNavigated OnCallGameOverMenuNavigated;

	UPROPERTY(BlueprintAssignable, category = "EventDispatchers")
	FOnPlayerPausedGame OnPlayerPausedGame;

	UPROPERTY(BlueprintAssignable, category = "EventDispatchers")
	FOnMenuNavigatedDown OnMenuNavigatedDown;

	UPROPERTY(BlueprintAssignable, category = "EventDispatchers")
	FOnMenuNavigatedUp OnMenuNavigatedUp;

	UPROPERTY(BlueprintAssignable, category = "EventDispatchers")
	FOnMenuNavigatedLeft OnMenuNavigatedLeft;

	UPROPERTY(BlueprintAssignable, category = "EventDispatchers")
	FOnMenuNavigatedRight OnMenuNavigatedRight;

	UPROPERTY(BlueprintAssignable, category = "EventDispatchers")
	FOnMenuEnter OnMenuEnter;

	UPROPERTY(BlueprintReadOnly)
	int currPlayerTeam;

	UPROPERTY(BlueprintReadOnly)
	ECBCharacterType currPlayerType;

	UFUNCTION()
	void CallMenuEnter();

	void AddNametagWidgetForPlayer(UWidgetComponent* nametagWidget);
	
protected:
	ACyberbowlCharacter* character;
	
	TArray<UWidgetComponent*> otherPlayerNametags;

	// Constants for the nametag widget height adjustment
	const float MinZWidgetPos = 150.f;
	const float MaxZWidgetPos = 550.f;
	const float MinPlayerDistance = 1000.f;
	const float MaxPlayerDistance = 15000.f;
};
