// Fill out your copyright notice in the Description page of Project Settings.


#include "TutorialLectures/EarthAbilityLecture.h"
#include "Actors/PlayBall.h"
#include "Character/Abilities/EarthAbility.h"
#include "Kismet/GameplayStatics.h"
#include "Character/CyberbowlCharacter.h"
#include "PlayerController/TutorialPlayerController.h"

void AEarthAbilityLecture::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEarthAbilityLecture::Enter()
{
	Super::Enter();

	tutorialCharacter = tutorialPlayerController->SwitchCharacterClass(earthCharacterClass);
	
	auto earthAbility = Cast<UEarthAbility>(tutorialCharacter->GetComponentByClass(UEarthAbility::StaticClass()));
	earthAbility->OnAActorLaunched.AddDynamic(this, &AEarthAbilityLecture::OnBallHit);

	ball->PlayBall();
}

void AEarthAbilityLecture::Exit()
{
	Super::Exit();
}

void AEarthAbilityLecture::BeginPlay()
{
	Super::BeginPlay();

	ball = Cast<APlayBall>(UGameplayStatics::GetActorOfClass(this, APlayBall::StaticClass()));
}

void AEarthAbilityLecture::SetupTasks()
{
	EnqueueTask(taskReadInstructions);
	EnqueueTask(taskLaunchSelf, 3);
	EnqueueTask(taskLaunchBall, 3);
}

void AEarthAbilityLecture::OnReadInstructions()
{
	AdvanceIfCurrentTask(taskReadInstructions);
}

void AEarthAbilityLecture::OnBallHit(AActor* launchedActor)
{
	if (Cast<ACyberbowlCharacter>(launchedActor))
	{
		AdvanceIfCurrentTask(taskLaunchSelf, 1.f);
		taskLaunchSelfAttempts++;
	}
	else if (Cast<APlayBall>(launchedActor))
	{
		AdvanceIfCurrentTask(taskLaunchBall, 1.f);
		taskLaunchBallAttempts++;
	}
}
