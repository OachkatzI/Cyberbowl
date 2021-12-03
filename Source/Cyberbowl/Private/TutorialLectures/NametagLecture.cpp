// Fill out your copyright notice in the Description page of Project Settings.


#include "TutorialLectures/NametagLecture.h"
#include "Character/CyberbowlCharacter.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerController/TutorialPlayerController.h"
#include "Engine/World.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraActor.h"
#include "Kismet/KismetMathLibrary.h"

void ANametagLecture::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ANametagLecture::Enter()
{
	Super::Enter();

	TArray<AActor*> playerStarts;
	UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), playerStarts);

	for (auto playerStart : playerStarts)
	{
		const auto start = Cast<APlayerStart>(playerStart);
		
		if (!start->PlayerStartTag.ToString().Contains("dummy"))
		{
			tutorialCharacter->SetActorTransform(start->GetTransform());			
			playerStarts.Remove(playerStart);
			
			break;
		}
	}

	for (int i = 0; i < 2; i++)
	{
		auto character = Cast<ACyberbowlCharacter>(GetWorld()->SpawnActor<AActor>(dummyCharacterClasses[i], playerStarts[i]->GetTransform()));
		character->TutorialNameTagSetup(i+1, characterTypes[i]);

		dummyCharacters.Add(character);
	}

	const FRotator lookAtDummy = UKismetMathLibrary::FindLookAtRotation(tutorialCharacter->GetActorLocation(), dummyCharacters[0]->GetActorLocation());
	tutorialPlayerController->SetControlRotation(lookAtDummy);
}

void ANametagLecture::Exit()
{
	Super::Exit();

	for (auto character : dummyCharacters)
	{
		character->Destroy();
	}
	
	dummyCharacters.Empty();
}

void ANametagLecture::BeginPlay()
{
	Super::BeginPlay();
}

void ANametagLecture::SetupTasks()
{
	EnqueueTask(taskPressedContinue);
}

void ANametagLecture::OnPressedContinue()
{
	AdvanceIfCurrentTask(taskPressedContinue);
}
