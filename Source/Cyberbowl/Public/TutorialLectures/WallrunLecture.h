// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TutorialLectures/TutorialLectureBase.h"
#include "WallrunLecture.generated.h"


UCLASS()
class CYBERBOWL_API AWallrunLecture : public ATutorialLectureBase
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
	FString taskWallrunLaunch = "taskWallrunLaunch";

	UPROPERTY(BlueprintReadOnly)
	int taskWallrunLaunchAttempts = 0;

	UFUNCTION()
	void OnWallrunEnd(float timeOnWall, bool launchedAway);
};
