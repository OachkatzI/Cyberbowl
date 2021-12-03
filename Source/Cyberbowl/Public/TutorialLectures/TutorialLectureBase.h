// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Widgets/TutorialWidgetBase.h"
#include "TutorialLectureBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLectureFinished);

class ATutorialPlayerController;
class ACyberbowlCharacter;
class ATutorialGameMode;

UCLASS(Abstract)
class CYBERBOWL_API ATutorialLectureBase : public AActor
{
	GENERATED_BODY()
	
public:	
	ATutorialLectureBase();

	virtual void Tick(float DeltaTime) override;

	virtual void Enter();

	virtual void Exit();

	UFUNCTION(BlueprintCallable)
	void AdvanceLecture();

	UFUNCTION(BlueprintCallable)
	void SkipLecture();

	UPROPERTY(BlueprintAssignable, category = "EventDispatchers")
	FOnLectureFinished OnLectureFinished;

	FString GetCurrentTask() { return currentTask; };

protected:
	virtual void BeginPlay() override;

	virtual void SetupTasks();

	void AdvanceIfCurrentTask(const FString& performedTask, float delayInSeconds = 1.f);

	void EnqueueTask(FString task, int attempts = 1);

	UPROPERTY(BlueprintReadOnly)
	FTimerHandle advanceTaskDelayHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<UTutorialWidgetBase>> widgetsList;

	UPROPERTY()
	UTutorialWidgetBase* currentWidget;

	ATutorialPlayerController* tutorialPlayerController;

	ACyberbowlCharacter* tutorialCharacter;

	ATutorialGameMode* tutorialGameMode;

private:
	TQueue<FString> lectureTasks;
	FString currentTask;
	FString oldTask;
};
