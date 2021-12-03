// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameModesAndInstances/InGameGameMode.h"
#include "Components/BoxComponent.h"
#include "Goal_Collider.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGoalScored, int, team);

UCLASS()
class CYBERBOWL_API AGoal_Collider : public AActor
{
	GENERATED_BODY()
	
public:	
	AGoal_Collider();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* Ball;

	UPROPERTY(BlueprintAssignable, category = "EventDispatchers")
	FOnGoalScored OnGoalScored;

	UPROPERTY(EditAnywhere)
	int TeamIndex;
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	int Points;
	
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UBoxComponent* BoxComponent;
};
