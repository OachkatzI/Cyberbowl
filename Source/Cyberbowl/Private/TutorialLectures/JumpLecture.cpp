// Fill out your copyright notice in the Description page of Project Settings.


#include "TutorialLectures/JumpLecture.h"
#include "Character/CyberbowlCharacter.h"

void AJumpLecture::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AJumpLecture::Enter()
{
	Super::Enter();
}

void AJumpLecture::Exit()
{
	Super::Exit();
}

void AJumpLecture::BeginPlay()
{
	Super::BeginPlay();

	tutorialCharacter->OnJump.AddDynamic(this, &AJumpLecture::OnJump);
	tutorialCharacter->OnDoubleJump.AddDynamic(this, &AJumpLecture::OnDoubleJump);

	movementComponent = Cast<UCBCharacterMovementComponent>(tutorialCharacter->GetMovementComponent());
}

void AJumpLecture::SetupTasks()
{
	EnqueueTask(taskJump, 3);
	EnqueueTask(taskDoubleJump, 3);
}

void AJumpLecture::OnJump()
{
	//WHY IS THIS WORKING?
	if (movementComponent->GetCBMovementMode() == ECBMovementMode::CBMOVE_Jump)
	{
		AdvanceIfCurrentTask(taskJump, 0.5f);
		taskJumpAttempts++;
	}
}

void AJumpLecture::OnDoubleJump()
{
	//protection against counting double jump attempts in jump lesson
	if (GetCurrentTask() == "taskDoubleJump")
	{
		AdvanceIfCurrentTask(taskDoubleJump, 0.5f);
		taskDoubleJumpAttempts++;
	}
}
