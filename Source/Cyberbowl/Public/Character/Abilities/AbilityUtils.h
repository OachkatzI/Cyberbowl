#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AbilityUtils.generated.h"

UCLASS()
class CYBERBOWL_API UAbilityUtils : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:
	/**
	 * Finds a target point on the floor within the arena (checks for collision with "StadiumWall" profile)
	 * @param targetPoint - reference where the found target point is saved
	 * @param wallOffset - how far away the target point should be away from the wall, if raycast is blocked by a wall
	 * @return false if a valid target point could not be found
	 */
    UFUNCTION(BlueprintCallable, Category = "AbilityUtils")
    static bool FindTargetPoint(UWorld* worldContext, FVector& targetPoint, FVector traceStart, FVector traceEnd, float wallOffset);

	/**
	 * Spawn a Niagara Effect that is attached to an actor and destroyed after the duration
	 * @param location - custom location in world space - if Zero the effect is spawned on the actor position
	*/
    UFUNCTION(BlueprintCallable, Category = "AbilityUtils")
    static void SpawnTimedEffect(UWorld* worldContext, AActor* attachActor, class UNiagaraSystem* effect, float duration, FVector location = FVector::ZeroVector);
};