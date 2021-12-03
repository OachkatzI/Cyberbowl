#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ILaunchable.generated.h"

// This class does not need to be modified.
UINTERFACE(BlueprintType)
class CYBERBOWL_API ULaunchable : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class CYBERBOWL_API ILaunchable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ILaunchable")
	void Launch(FVector direction, float forceHorizontal, float forceVertical, class UNiagaraSystem* launchEffect, float launchEffectDuration);
};
