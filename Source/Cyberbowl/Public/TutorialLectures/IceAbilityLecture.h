// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TutorialLectures/TutorialLectureBase.h"
#include "IceAbilityLecture.generated.h"

class APlayBall;
class AGoal_Collider;

UCLASS()
class CYBERBOWL_API AIceAbilityLecture : public ATutorialLectureBase
{
	GENERATED_BODY()
	
public:
	void Tick(float DeltaTime) override;
	void Enter() override;
	void Exit() override;
	
protected:
	void BeginPlay() override;
	void SetupTasks() override;

	// Lecture tasks
	FString taskReadInstructions = "taskReadInstructions";
	FString taskFreezeBall = "taskFreezeBall";

	UPROPERTY(BlueprintReadOnly)
	int taskFreezeBallAttempts = 0;

	UFUNCTION(BlueprintCallable)
	void OnReadInstructions();

	UFUNCTION()
	void OnBallFrozen();

	UFUNCTION()
	void OnGoalScored(int teamIndex);

	UFUNCTION(BlueprintCallable)
	void LaunchBall() const;

	APlayBall* ball;

	AActor* ballLaunchStartLocation;

	AActor* playerStartLocation;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ACyberbowlCharacter> iceCharacterClass;

	bool bSwitchLaunchDirection;

	UPROPERTY(BlueprintReadOnly)
	FTimerHandle restartBallDelayHandle;
};
