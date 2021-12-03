// Fill out your copyright notice in the Description page of Project Settings.


#include "TutorialLectures/OutroLecture.h"
#include "PlayerController/TutorialPlayerController.h"

void AOutroLecture::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AOutroLecture::Enter()
{
	Super::Enter();
}

void AOutroLecture::Exit()
{
	Super::Exit();
}

void AOutroLecture::BeginPlay()
{
	Super::BeginPlay();
}

void AOutroLecture::SetupTasks()
{
	EnqueueTask(taskPressedContinue);
}

void AOutroLecture::OnPressedContinue()
{
	AdvanceIfCurrentTask(taskPressedContinue);
}
