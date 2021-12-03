// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WMainMenu.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLobbyEntered);

UCLASS()
class CYBERBOWL_API UWMainMenu : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable, BlueprintCallable, category = "EventDispatchers")
	FOnLobbyEntered LobbyEntered;
	
};
