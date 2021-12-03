// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WLobby.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerReadyUnready);

UCLASS()
class CYBERBOWL_API UWLobby : public UUserWidget
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintAssignable, BlueprintCallable, category = "EventDispatchers")
	FOnPlayerReadyUnready PlayerReadyUnready;
	
};
