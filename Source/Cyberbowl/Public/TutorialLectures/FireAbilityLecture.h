// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TutorialLectures/TutorialLectureBase.h"
#include "FireAbilityLecture.generated.h"

class APlayBall;
class AGoal_Collider;

UCLASS()
class CYBERBOWL_API AFireAbilityLecture : public ATutorialLectureBase
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
	FString taskBlockBall = "taskBlockBall";

	UPROPERTY(BlueprintReadOnly)
	int taskBlockBallAttempts = 0;
	
	UFUNCTION(BlueprintCallable)
	void OnReadInstructions();
	
	UFUNCTION()
	void OnBallBlocked(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void OnGoalScored(int teamIndex);

	UFUNCTION(BlueprintCallable)
	void LaunchBall() const;

	APlayBall* ball;

	AActor* ballLaunchStartLocation;

	AActor* playerStartLocation;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ACyberbowlCharacter> fireCharacterClass;

	bool bSwitchLaunchDirection;
};
