// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "PlayerController/FPlayerInfo.h"
#include "CyberbowlGameInstance.generated.h"


/**
 * 
 */
UCLASS()
class CYBERBOWL_API UCyberbowlGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	TMap<int, FPlayerInfo> PlayerInfo;

	UPROPERTY()
	int TotalPlayers;

	UPROPERTY()
	int SpawnedCharacters;

	UFUNCTION()
	void CheckAllCharactersSpawned();
};
