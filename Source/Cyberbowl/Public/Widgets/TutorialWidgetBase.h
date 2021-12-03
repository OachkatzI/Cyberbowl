// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TutorialWidgetBase.generated.h"

class ATutorialLectureBase;

UCLASS()
class CYBERBOWL_API UTutorialWidgetBase : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetTutorialLecture(ATutorialLectureBase* lecture);
	
protected:
	UPROPERTY(BlueprintReadOnly)
	ATutorialLectureBase* tutorialLecture;
};
