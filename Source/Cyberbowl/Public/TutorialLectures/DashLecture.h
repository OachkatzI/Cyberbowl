// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TutorialLectures/TutorialLectureBase.h"
#include "DashLecture.generated.h"

class UCyberbowlCharacterAnimInstance;

UCLASS()
class CYBERBOWL_API ADashLecture : public ATutorialLectureBase
{
	GENERATED_BODY()
	
public:
	void Tick(float DeltaTime) override;
	void Enter() override;
	void Exit() override;
	
protected:
	void BeginPlay() override;
	void SetupTasks() override;

	bool bJustJumped;
	float timeAfterJump;
	
	// Lecture tasks
	FString taskRegularDash = "taskRegularDash";
	FString taskJumpDash = "taskJumpDash";
	FString taskVerticalDash = "taskVerticalDash";

	UPROPERTY(BlueprintReadOnly)
	int taskRegularDashAttempts = 0;

	UPROPERTY(BlueprintReadOnly)
	int taskJumpDashAttempts = 0;

	UPROPERTY(BlueprintReadOnly)
	int taskVerticalDashAttempts = 0;
	
	UFUNCTION()
	void OnRegularDash();

	UFUNCTION()
	void OnVerticalDash();

	UCyberbowlCharacterAnimInstance* animInstance;
};
