// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TutorialLectures/TutorialLectureBase.h"
#include "PlayerController/FPlayerInfo.h"
#include "NametagLecture.generated.h"

UCLASS()
class CYBERBOWL_API ANametagLecture : public ATutorialLectureBase
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
	FString taskPressedContinue = "taskPressedContinue";
	void OnPressedContinue();

	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<ACyberbowlCharacter>> dummyCharacterClasses;

	TArray<ECBCharacterType> characterTypes{ ECBCharacterType::CBCHRACTERTYPE_AIR, ECBCharacterType::CBCHRACTERTYPE_EARTH };

	TArray<ACyberbowlCharacter*> dummyCharacters;
};
