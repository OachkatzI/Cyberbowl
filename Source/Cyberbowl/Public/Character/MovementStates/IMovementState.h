// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IMovementState.generated.h"

// This class does not need to be modified.
UINTERFACE(BlueprintType)
class UIMovementState : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class CYBERBOWL_API IIMovementState
{
	GENERATED_BODY()

public:
	//Call this when State is created, for assigning members and such
	UFUNCTION(Category = "MovementState")
	virtual void InitializeState() = 0;

	UFUNCTION(Category = "MovementState")
	virtual void Activate() = 0;
	
	UFUNCTION(Category = "MovementState")
	virtual void Deactivate() = 0;
};
