// Fill out your copyright notice in the Description page of Project Settings.


#include "TutorialLectures/IntroLecture.h"
#include "PlayerController/TutorialPlayerController.h"

void AIntroLecture::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AIntroLecture::Enter()
{
	Super::Enter();
}

void AIntroLecture::Exit()
{
	Super::Exit();
}

void AIntroLecture::BeginPlay()
{
	Super::BeginPlay();
}

void AIntroLecture::SetupTasks()
{
	EnqueueTask(taskPressedContinue);
}

void AIntroLecture::OnPressedContinue()
{
	AdvanceIfCurrentTask(taskPressedContinue);
}
