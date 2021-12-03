// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TutorialLectures/TutorialLectureBase.h"
#include "GoalLecture.generated.h"

class APlayBall;

UCLASS()
class CYBERBOWL_API AGoalLecture : public ATutorialLectureBase
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
	FString taskScoredGoal = "taskScoredGoal";

	UFUNCTION()
	void OnGoalScored(int teamIndex);

	UPROPERTY(BlueprintReadOnly)
	int taskScoredGoalAttempts = 0;
	
	void ResetBall() const;

	APlayBall* ball;

	AActor* ballLaunchStartLocation;
};
