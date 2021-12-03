#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IFreezeable.generated.h"

// This class does not need to be modified.
UINTERFACE(BlueprintType)
class CYBERBOWL_API UFreezeable : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class CYBERBOWL_API IFreezeable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "IFreezable")
	void Freeze(AActor* instigtr);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "IFreezable")
	void UnFreeze();
};
