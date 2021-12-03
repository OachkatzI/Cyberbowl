// Fill out your copyright notice in the Description page of Project Settings.


#include "TutorialLectures/AirAbilityLecture.h"
#include "Actors/PlayBall.h"
#include "Character/Abilities/AirAbility.h"
#include "Kismet/GameplayStatics.h"
#include "Stadium/Goal_Collider.h"
#include "PlayerController/TutorialPlayerController.h"
#include "Character/CyberbowlCharacter.h"
#include "TimerManager.h"

void AAirAbilityLecture::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AAirAbilityLecture::Enter()
{
	Super::Enter();

	tutorialCharacter->SetActorLocation(FVector(playerStartLocation->GetActorLocation().X, playerStartLocation->GetActorLocation().Y, tutorialCharacter->GetActorLocation().Z));
	ball->SetActorLocation(ballLaunchStartLocation->GetActorLocation());

	tutorialCharacter = tutorialPlayerController->SwitchCharacterClass(airCharacterClass);
	auto airAbility = Cast<UAirAbility>(tutorialCharacter->GetComponentByClass(UAirAbility::StaticClass()));
	airAbility->OnGrabModeExitByPush.AddDynamic(this, &AAirAbilityLecture::OnBallSucced);
}

void AAirAbilityLecture::Exit()
{
	Super::Exit();
}

void AAirAbilityLecture::BeginPlay()
{
	Super::BeginPlay();

	ball = Cast<APlayBall>(UGameplayStatics::GetActorOfClass(this, APlayBall::StaticClass()));

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
		goal->OnGoalScored.AddDynamic(this, &AAirAbilityLecture::OnGoalScored);
	}
	actors.Empty();
}

void AAirAbilityLecture::SetupTasks()
{
	EnqueueTask(taskReadInstructions);
	EnqueueTask(taskGrabAndThrow, 3);
}

void AAirAbilityLecture::OnReadInstructions()
{
	AdvanceIfCurrentTask(taskReadInstructions);
}

void AAirAbilityLecture::OnBallSucced()
{
	AdvanceIfCurrentTask(taskGrabAndThrow, 1.f);
	taskGrabAndThrowAttempts++;
	bSwitchLaunchDirection = !bSwitchLaunchDirection;
	GetWorld()->GetTimerManager().SetTimer(restartBallDelayHandle, this, &AAirAbilityLecture::LaunchBall, 1.f, false);
}

void AAirAbilityLecture::OnGoalScored(int teamIndex)
{
	LaunchBall();
}

void AAirAbilityLecture::LaunchBall() const
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
