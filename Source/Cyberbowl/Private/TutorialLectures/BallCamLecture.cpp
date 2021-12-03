// Fill out your copyright notice in the Description page of Project Settings.


#include "TutorialLectures/BallCamLecture.h"
#include "Character/CyberbowlCharacter.h"

void ABallCamLecture::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABallCamLecture::Enter()
{
	Super::Enter();
}

void ABallCamLecture::Exit()
{
	Super::Exit();
}

void ABallCamLecture::BeginPlay()
{
	Super::BeginPlay();

	tutorialCharacter->OnToggledBallCam.AddDynamic(this, &ABallCamLecture::OnBallCamToggled);
}

void ABallCamLecture::SetupTasks()
{
	EnqueueTask(taskBallCamToggled, 3);
}

void ABallCamLecture::OnBallCamToggled()
{
	//Set delay very low since this can be spamed and then there is a higher score then 3
	AdvanceIfCurrentTask(taskBallCamToggled, 0.1f);
	taskBallCamToggledAttempts++;
}
