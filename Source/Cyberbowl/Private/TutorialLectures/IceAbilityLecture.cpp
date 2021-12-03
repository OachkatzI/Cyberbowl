// Fill out your copyright notice in the Description page of Project Settings.


#include "TutorialLectures/IceAbilityLecture.h"
#include "Actors/PlayBall.h"
#include "Kismet/GameplayStatics.h"
#include "Stadium/Goal_Collider.h"
#include "PlayerController/TutorialPlayerController.h"
#include "Character/CyberbowlCharacter.h"
#include "TimerManager.h"

void AIceAbilityLecture::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AIceAbilityLecture::Enter()
{
	Super::Enter();
	
	tutorialCharacter->SetActorLocation(FVector(playerStartLocation->GetActorLocation().X, playerStartLocation->GetActorLocation().Y, tutorialCharacter->GetActorLocation().Z));
	ball->SetActorLocation(ballLaunchStartLocation->GetActorLocation());
	
	tutorialCharacter = tutorialPlayerController->SwitchCharacterClass(iceCharacterClass);
}

void AIceAbilityLecture::Exit()
{
	Super::Exit();

	ball->PlayBall();
}

void AIceAbilityLecture::BeginPlay()
{
	Super::BeginPlay();

	ball = Cast<APlayBall>(UGameplayStatics::GetActorOfClass(this, APlayBall::StaticClass()));
	ball->OnBallFrozen.AddDynamic(this, &AIceAbilityLecture::OnBallFrozen);

	TArray<AActor*> actors;

	UGameplayStatics::GetAllActorsWithTag(this, "FireLecturePlayerStart", actors);
	playerStartLocation = actors[0];
	actors.Empty();

	UGameplayStatics::GetAllActorsWithTag(this, "FireLectureBallStart", actors);
	ballLaunchStartLocation = actors[0];
	actors.Empty();

	UGameplayStatics::GetAllActorsOfClass(this, AGoal_Collider::StaticClass(), actors);
	for (auto goalActor : actors)
	{
		auto goal = Cast<AGoal_Collider>(goalActor);
		goal->OnGoalScored.AddDynamic(this, &AIceAbilityLecture::OnGoalScored);
	}

	actors.Empty();
}

void AIceAbilityLecture::SetupTasks()
{
	EnqueueTask(taskReadInstructions);
	EnqueueTask(taskFreezeBall, 3);
}

void AIceAbilityLecture::OnReadInstructions()
{
	AdvanceIfCurrentTask(taskReadInstructions);
}

void AIceAbilityLecture::OnBallFrozen()
{
	AdvanceIfCurrentTask(taskFreezeBall, 1.f);
	taskFreezeBallAttempts++;
	bSwitchLaunchDirection = !bSwitchLaunchDirection;
	GetWorld()->GetTimerManager().SetTimer(restartBallDelayHandle, this, &AIceAbilityLecture::LaunchBall, 1.f, false);
}

void AIceAbilityLecture::OnGoalScored(int teamIndex)
{
	LaunchBall();
}

void AIceAbilityLecture::LaunchBall() const
{
	ball->StopBall();
	ball->PlayBall();
	ball->SetActorLocation(ballLaunchStartLocation->GetActorLocation());

	FVector launchDirection;
	if (bSwitchLaunchDirection)
	{
		launchDirection = { 0, 1, 0 };
	}
	else
	{
		launchDirection = { 0, -1, 0 };
	}

	ball->PushBall(nullptr, 4000.f, launchDirection);
}
