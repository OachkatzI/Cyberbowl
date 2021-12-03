// Fill out your copyright notice in the Description page of Project Settings.


#include "TutorialLectures/FireAbilityLecture.h"
#include "Actors/PlayBall.h"
#include "Character/Abilities/Firewall.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "PlayerController/TutorialPlayerController.h"
#include "Stadium/Goal_Collider.h"
#include "Character/CyberbowlCharacter.h"

void AFireAbilityLecture::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AFireAbilityLecture::Enter()
{
	Super::Enter();

	tutorialCharacter->SetActorLocation(FVector(playerStartLocation->GetActorLocation().X, playerStartLocation->GetActorLocation().Y, tutorialCharacter->GetActorLocation().Z));
	ball->SetActorLocation(ballLaunchStartLocation->GetActorLocation());

	tutorialCharacter = tutorialPlayerController->SwitchCharacterClass(fireCharacterClass);
}

void AFireAbilityLecture::Exit()
{
	Super::Exit();
}

void AFireAbilityLecture::BeginPlay()
{
	Super::BeginPlay();

	ball = Cast<APlayBall>(UGameplayStatics::GetActorOfClass(this, APlayBall::StaticClass()));
	ball->OnActorHit.AddDynamic(this, &AFireAbilityLecture::OnBallBlocked);

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
		goal->OnGoalScored.AddDynamic(this, &AFireAbilityLecture::OnGoalScored);
	}
	
	actors.Empty();
}

void AFireAbilityLecture::SetupTasks()
{
	EnqueueTask(taskReadInstructions, 1);
	EnqueueTask(taskBlockBall, 3);
}

void AFireAbilityLecture::OnReadInstructions()
{
	AdvanceIfCurrentTask(taskReadInstructions);
}

void AFireAbilityLecture::OnBallBlocked(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	if (Cast<AFirewall>(OtherActor))
	{
		AdvanceIfCurrentTask(taskBlockBall, 1.f);
		taskBlockBallAttempts++;
		bSwitchLaunchDirection = !bSwitchLaunchDirection;
		LaunchBall();
	}
}

void AFireAbilityLecture::OnGoalScored(int teamIndex)
{
	LaunchBall();
}

void AFireAbilityLecture::LaunchBall() const
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
