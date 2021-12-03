// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameplayTagContainer.h"
#include "GameModesAndInstances/MainMenuGameMode.h"
#include "Widgets/WLobby.h"
#include "Character/CyberbowlCharacter.h"
#include "MainMenuPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPreviousCharacterSelected);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNextCharacterSelected);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPreviousTeamSelected);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNextTeamSelected);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSelectionApplied);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLobbyJoined);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMainMenuNavigated);

UCLASS()
class CYBERBOWL_API AMainMenuPlayerController : public APlayerController
{
	GENERATED_BODY()

	void SetupInputComponent() override;

public:

	UPROPERTY(BlueprintAssignable, category = "EventDispatchers")
	FOnPreviousCharacterSelected PreviousCharacterSelected;

	UPROPERTY(BlueprintAssignable, category = "EventDispatchers")
	FOnNextCharacterSelected NextCharacterSelected;

	UPROPERTY(BlueprintAssignable, category = "EventDispatchers")
	FOnPreviousTeamSelected PreviousTeamSelected;

	UPROPERTY(BlueprintAssignable, category = "EventDispatchers")
	FOnNextTeamSelected NextTeamSelected;

	UPROPERTY(BlueprintAssignable, category = "EventDispatchers")
	FOnSelectionApplied SelectionApplied;

	UPROPERTY(BlueprintAssignable, category = "EventDispatchers")
	FOnLobbyJoined LobbyJoined;

	UPROPERTY(BlueprintAssignable, category = "EventDispatchers")
	FOnMainMenuNavigated MainMenuNavigated;

	UFUNCTION(BlueprintCallable)
	void PushToMenuStack(UUserWidget* newWidget);

	UFUNCTION(BlueprintCallable)
	void PopFromMenuStack();

	UFUNCTION(BlueprintCallable)
	bool GetIsAssigned() const;

	UFUNCTION(BlueprintCallable)
	void SetIsAssigned(bool assigned);

	UFUNCTION(BlueprintCallable)
	UWLobby* GetLobbyWidget() const;

	UFUNCTION(BlueprintCallable)
	void SetLobbyWidget(UWLobby* lobbyWidget);

	UFUNCTION(BlueprintCallable)
	UUserWidget* GetLobbyNotJoinedWidget() const;

	UFUNCTION(BlueprintCallable)
	void SetLobbyNotJoinedWidget(UUserWidget* lobbyNotJoinedWidget);

	UFUNCTION(BlueprintCallable)
	ACyberbowlCharacter* GetCharacterPreviewDummy() const;

	UFUNCTION(BlueprintCallable)
	void SetCharacterPreviewDummy(ACyberbowlCharacter* characterPreviewDummy);
	
protected:

	UPROPERTY()
	bool bIsAssigned;

	UPROPERTY()
	TArray<UUserWidget*> widgetStack;

	UPROPERTY()
	AMainMenuGameMode* mainMenuGameMode;

	UPROPERTY()
	UWLobby* LobbyWidget;

	UPROPERTY()
	UUserWidget* LobbyNotJoinedWidget;

	UPROPERTY()
	ACyberbowlCharacter* CharacterPreviewDummy;
	
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void OnIndexReady();

	UFUNCTION(BlueprintCallable)
	void FPreviousCharacterSelected();

	UFUNCTION(BlueprintCallable)
	void FNextCharacterSelected();

	UFUNCTION(BlueprintCallable)
	void FPreviousTeamSelected();

	UFUNCTION(BlueprintCallable)
	void FNextTeamSelected();

	UFUNCTION(BlueprintCallable)
	void FSelectionApplied();

	UFUNCTION(BlueprintCallable)
	void FLobbyJoined();

	UFUNCTION(BlueprintCallable)
	void FMainMenuNavigated();
	
};
