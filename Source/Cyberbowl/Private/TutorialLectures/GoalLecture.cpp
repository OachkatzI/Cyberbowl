// Fill out your copyright notice in the Description page of Project Settings.


#include "TutorialLectures/GoalLecture.h"


#include "Actors/PlayBall.h"
#include "Kismet/GameplayStatics.h"
#include "Stadium/Goal_Collider.h"

void AGoalLecture::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGoalLecture::Enter()
{
	Super::Enter();
}

void AGoalLecture::Exit()
{
	Super::Exit();
}

void AGoalLecture::BeginPlay()
{
	Super::BeginPlay();

	ball = Cast<APlayBall>(UGameplayStatics::GetActorOfClass(this, APlayBall::StaticClass()));
	ball->PlayBall();

	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsWithTag(this, "FireLectureBallStart", actors);
	ballLaunchStartLocation = actors[0];
	
	TArray<AActor*> goalColliders;
	UGameplayStatics::GetAllActorsOfClass(this, AGoal_Collider::StaticClass(), goalColliders);

	for (auto goal : goalColliders)
	{
		Cast<AGoal_Collider>(goal)->OnGoalScored.AddDynamic(this, &AGoalLecture::OnGoalScored);
	}
}

void AGoalLecture::SetupTasks()
{
	EnqueueTask(taskScoredGoal, 3);
}

void AGoalLecture::OnGoalScored(int teamIndex)
{
	AdvanceIfCurrentTask(taskScoredGoal, 0);
	taskScoredGoalAttempts++;
	ResetBall();
}

void AGoalLecture::ResetBall() const
{	
	ball->SetActorLocation(ballLaunchStartLocation->GetActorLocation());
	ball->StopBall();
	ball->PlayBall();
}
