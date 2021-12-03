// Fill out your copyright notice in the Description page of Project Settings.


#include "TutorialLectures/WallrunLecture.h"

#include <string>

#include "Character/CyberbowlCharacter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Character/CBCharacterMovementComponent.h"

void AWallrunLecture::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWallrunLecture::Enter()
{
	Super::Enter();
}

void AWallrunLecture::Exit()
{
	Super::Exit();
}

void AWallrunLecture::BeginPlay()
{
	Super::BeginPlay();

	Cast<UCBCharacterMovementComponent>(tutorialCharacter->GetMovementComponent())->OnWallrunEnd.AddDynamic(this, &AWallrunLecture::OnWallrunEnd);
}

void AWallrunLecture::SetupTasks()
{
	EnqueueTask(taskWallrunLaunch, 3);
}

void AWallrunLecture::OnWallrunEnd(float timeOnWall, bool launchedAway)
{
	UKismetSystemLibrary::PrintString(this, std::to_string(timeOnWall).c_str());
	if (launchedAway && timeOnWall >= 1.f)
	{
		AdvanceIfCurrentTask(taskWallrunLaunch, 0.5f);
		taskWallrunLaunchAttempts++;
	}
}
