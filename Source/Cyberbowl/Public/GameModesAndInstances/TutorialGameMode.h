// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Containers/Queue.h"
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TutorialGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTutorialFinished);

class ATutorialLectureBase;

UCLASS()
class CYBERBOWL_API ATutorialGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ATutorialGameMode(const FObjectInitializer& ObjectInitializer);
	
	UFUNCTION(BlueprintCallable)
	void AdvanceTutorial();

	void BeginPlay() override;

	void Tick(float DeltaSeconds) override;

	UPROPERTY(BlueprintAssignable, category = "EventDispatchers")
	FOnTutorialFinished OnTutorialFinished;

protected:
	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<ATutorialLectureBase>> lecturesList;

	UPROPERTY()
	ATutorialLectureBase* currentLecture;
};
