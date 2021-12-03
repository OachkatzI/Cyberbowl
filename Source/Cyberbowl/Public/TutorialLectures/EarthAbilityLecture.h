// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TutorialLectures/TutorialLectureBase.h"
#include "EarthAbilityLecture.generated.h"

class APlayBall;
class AGoal_Collider;

UCLASS()
class CYBERBOWL_API AEarthAbilityLecture : public ATutorialLectureBase
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
	FString taskLaunchSelf = "taskLaunchSelf";
	FString taskLaunchBall = "taskLaunchBall";

	UPROPERTY(BlueprintReadOnly)
	int taskLaunchBallAttempts = 0;

	UPROPERTY(BlueprintReadOnly)
	int taskLaunchSelfAttempts = 0;

	UFUNCTION(BlueprintCallable)
	void OnReadInstructions();

	UFUNCTION()
	void OnBallHit(AActor* launchedActor);

	APlayBall* ball;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ACyberbowlCharacter> earthCharacterClass;
};
