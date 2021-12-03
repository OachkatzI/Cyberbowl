// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "FPlayerInfo.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class ECBCharacterType : uint8
{
	CBCHRACTERTYPE_FIRE UMETA(DisplayName = "Fire"),
	CBCHRACTERTYPE_EARTH UMETA(DisplayName = "Earth"),
	CBCHRACTERTYPE_ICE UMETA(DisplayName = "Ice"),
	CBCHRACTERTYPE_AIR UMETA(DisplayName = "Air"),
};

inline const char* ToCharacterName(ECBCharacterType type)
{
	switch (type)
	{
		case ECBCharacterType::CBCHRACTERTYPE_FIRE:
			return "HAPALA";
		case ECBCharacterType::CBCHRACTERTYPE_EARTH:
			return "JISHIN";
		case ECBCharacterType::CBCHRACTERTYPE_ICE:
			return "FREY";
		case ECBCharacterType::CBCHRACTERTYPE_AIR:
			return "ZEFYRA";
		default:
			return "Unknown";
	}
}

USTRUCT(BlueprintType)
struct FPlayerInfo
{
	GENERATED_BODY()

	FPlayerInfo()
	{
		Team = 1;
		CharacterType = ECBCharacterType::CBCHRACTERTYPE_FIRE;
	}

	UPROPERTY(BlueprintReadWrite)
	int Team;
	
	UPROPERTY(BlueprintReadWrite)
	ECBCharacterType CharacterType;
	
};

UCLASS()
class CYBERBOWL_API UFPlayerInfo : public UObject
{
	GENERATED_BODY()
	
};
