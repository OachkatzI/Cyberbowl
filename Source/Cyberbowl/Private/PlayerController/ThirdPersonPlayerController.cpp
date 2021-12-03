// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerController/ThirdPersonPlayerController.h"

#include <string>

#include "GameModesAndInstances/CyberbowlGameInstance.h"
#include "Character/BallCamComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "GameModesAndInstances/InGameGameMode.h"
#include "Character/CyberbowlCharacter.h"
#include "Components/WidgetComponent.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Kismet/KismetMathLibrary.h"
#include "Widgets/WNameTag.h"

void AThirdPersonPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	SetAsLocalPlayerController();
	SpawnActors();
	//Disable movement til the countdown when starting the game ends.
	character->DisableInput(this);
}

void AThirdPersonPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateNametagPositions();
}

void AThirdPersonPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("MenuNavigationDown", IE_Pressed, this, &AThirdPersonPlayerController::CallGameOverMenuNavigated);
	InputComponent->BindAction("MenuNavigationUp", IE_Pressed, this, &AThirdPersonPlayerController::CallGameOverMenuNavigated);
	InputComponent->BindAction("PauseGame", IE_Pressed, this, &AThirdPersonPlayerController::CallPlayerPausedGame);
	InputComponent->BindAction("MenuNavigationDown", IE_Pressed, this, &AThirdPersonPlayerController::CallMenuNavigationDown);
	InputComponent->BindAction("MenuNavigationUp", IE_Pressed, this, &AThirdPersonPlayerController::CallMenuNavigationUp);
	InputComponent->BindAction("MenuNavigationRight", IE_Pressed, this, &AThirdPersonPlayerController::CallMenuNavigationRight);
	InputComponent->BindAction("MenuNavigationLeft", IE_Pressed, this, &AThirdPersonPlayerController::CallMenuNavigationLeft);
}

void AThirdPersonPlayerController::SpawnActors()
{
	UCyberbowlGameInstance* gameInstance = Cast<UCyberbowlGameInstance>(GetGameInstance());
	TMap<int, FPlayerInfo> playerInfo = gameInstance->PlayerInfo;
	FPlayerInfo* currPlayerInfo = playerInfo.Find(UGameplayStatics::GetPlayerControllerID(this));

	//For Debug Purposes Only
	//Todo: Delete later on!
	if (!currPlayerInfo)
	{
		FPlayerInfo currPlayerInfoStruct;
		currPlayerInfo = &currPlayerInfoStruct;
		currPlayerInfo->Team = 1;
		currPlayerInfo->CharacterType = ECBCharacterType::CBCHRACTERTYPE_FIRE;
	}
	
	currPlayerTeam = currPlayerInfo->Team;
	currPlayerType = currPlayerInfo->CharacterType;
	
	TArray<AActor*> playerStarts;

	AInGameGameMode* gameMode = Cast<AInGameGameMode>(GetWorld()->GetAuthGameMode());
	APlayerStart* playerSpawn = gameMode->GetPlayerStart(currPlayerTeam);

	character = nullptr;
	FVector spawnTransform = playerSpawn->GetTransform().GetLocation();
	FRotator spawnRotation = playerSpawn->GetActorRotation();
	
	switch (currPlayerType)
	{
	case ECBCharacterType::CBCHRACTERTYPE_FIRE:
		character = GetWorld()->SpawnActor<ACyberbowlCharacter>(fireClass, spawnTransform, spawnRotation);
		break;

	case ECBCharacterType::CBCHRACTERTYPE_EARTH:
		character = GetWorld()->SpawnActor<ACyberbowlCharacter>(earthClass, spawnTransform, spawnRotation);
		break;

	case ECBCharacterType::CBCHRACTERTYPE_ICE:
		character = GetWorld()->SpawnActor<ACyberbowlCharacter>(iceClass, spawnTransform, spawnRotation);
		break;

	case ECBCharacterType::CBCHRACTERTYPE_AIR:
		character = GetWorld()->SpawnActor<ACyberbowlCharacter>(airClass, spawnTransform, spawnRotation);
		break;
	}

	Possess(character);

	gameInstance->SpawnedCharacters++;
	gameInstance->CheckAllCharactersSpawned();

	UUserWidget* baseWidget = CreateWidget<UUserWidget>(this, baseHudClass);
	baseWidget->AddToPlayerScreen();



	gameMode->StartGamePlay.AddDynamic(this, &AThirdPersonPlayerController::OnStartGamePlay);
	gameMode->GoalScored.AddDynamic(this, &AThirdPersonPlayerController::OnGoalScored);
	gameMode->Regroup.AddDynamic(this, &AThirdPersonPlayerController::OnRegroup);
	gameMode->EndGame.AddDynamic(this, &AThirdPersonPlayerController::OnEndGame);
}

void AThirdPersonPlayerController::SetupNameTagWidgets()
{
	TArray<UWidgetComponent*, FDefaultAllocator> widgetComponents;
	character->GetComponents<UWidgetComponent, FDefaultAllocator>(widgetComponents);

	// We need to re-activate the components, since they are being deactivated based on the player count last round
	for (auto widgetComp : widgetComponents)
	{
		widgetComp->Activate();
	}

	TArray<AActor*> playerControllers;
	UGameplayStatics::GetAllActorsOfClass(this, AThirdPersonPlayerController::StaticClass(), playerControllers);

	for (auto controller : playerControllers)
	{
		const auto playerController = Cast<AThirdPersonPlayerController>(controller);

		if (playerController == this)
			continue;
		
		UWidgetComponent* widgetComponent = widgetComponents.Pop();
		UWNameTag* nameTagWidget = Cast<UWNameTag>(widgetComponent->GetUserWidgetObject());
		UButton* nameplate = Cast<UButton>(nameTagWidget->GetWidgetFromName("TeamColorButton"));
		UImage* arrowIndicator = Cast<UImage>(nameTagWidget->GetWidgetFromName("ArrowIndicator"));
		nameTagWidget->CharacterName = ToCharacterName(currPlayerType);	

		if (currPlayerTeam == 1)
		{
			nameplate->SetBackgroundColor(FLinearColor(0.97, 0.06, 0.38, 0.5));
			arrowIndicator->SetBrushFromTexture(arrowIndicatorTeamRed);
		}
		else
		{
			nameplate->SetBackgroundColor(FLinearColor(0.04, 0.76, 0.78, 0.5));
			arrowIndicator->SetBrushFromTexture(arrowIndicatorTeamBlue);
		}

		widgetComponent->SetOwnerPlayer(playerController->GetLocalPlayer());
		playerController->AddNametagWidgetForPlayer(widgetComponent);
		nameTagWidget->SetOwningPlayer(playerController);
		nameTagWidget->SetOwningLocalPlayer(playerController->GetLocalPlayer());
		nameTagWidget->IsAssigned = true;
	}

	for (auto widgetComp : widgetComponents)
	{
		const bool isAssigned = Cast<UWNameTag>(widgetComp->GetUserWidgetObject())->IsAssigned;
		if (!isAssigned)
		{
			widgetComp->Deactivate();
		}
	}
}

void AThirdPersonPlayerController::OnStartGamePlay()
{
	character->EnableInput(this);
}

void AThirdPersonPlayerController::OnGoalScored()
{
	character->DisableInput(this);
}

void AThirdPersonPlayerController::OnRegroup()
{
	AInGameGameMode* gameMode = Cast<AInGameGameMode>(GetWorld()->GetAuthGameMode());
	APlayerStart* playerSpawn = gameMode->GetPlayerStart(currPlayerTeam);

	FVector spawnTransform = playerSpawn->GetTransform().GetLocation();
	FRotator spawnRotation = playerSpawn->GetActorRotation();
	character->SetActorLocation(spawnTransform);
	character->SetActorRotation(spawnRotation);
	UBallCamComponent* ballCam = Cast<UBallCamComponent>(character->FindComponentByClass<UBallCamComponent>());
	ballCam->DoNotFollow();

	character->GetController()->SetControlRotation(spawnRotation);
}

void AThirdPersonPlayerController::OnEndGame()
{
	character->DisableInput(this);
}

void AThirdPersonPlayerController::CallGameOverMenuNavigated()
{
	OnCallGameOverMenuNavigated.Broadcast();
}

void AThirdPersonPlayerController::CallPlayerPausedGame()
{
	OnPlayerPausedGame.Broadcast(UGameplayStatics::GetPlayerControllerID(this));
}

void AThirdPersonPlayerController::CallMenuNavigationDown()
{
	if (Cast<AInGameGameMode>(UGameplayStatics::GetGameMode(this))->GetIsPaused())
	{
		OnMenuNavigatedDown.Broadcast();
	}
}

void AThirdPersonPlayerController::CallMenuNavigationUp()
{
	if (Cast<AInGameGameMode>(UGameplayStatics::GetGameMode(this))->GetIsPaused())
	{
		OnMenuNavigatedUp.Broadcast();
	}
}

void AThirdPersonPlayerController::CallMenuNavigationLeft()
{
	if (Cast<AInGameGameMode>(UGameplayStatics::GetGameMode(this))->GetIsPaused())
	{
		OnMenuNavigatedLeft.Broadcast();
	}
}

void AThirdPersonPlayerController::CallMenuNavigationRight()
{
	if (Cast<AInGameGameMode>(UGameplayStatics::GetGameMode(this))->GetIsPaused())
	{
		OnMenuNavigatedRight.Broadcast();
	}
}

void AThirdPersonPlayerController::UpdateNametagPositions()
{
	for (auto nametag : otherPlayerNametags)
	{
		const auto distanceToOtherPlayer = UKismetMathLibrary::Vector_Distance(character->GetActorLocation(), nametag->GetComponentLocation());
		const float heightAdjustment = (distanceToOtherPlayer - MinPlayerDistance) * (MaxZWidgetPos - MinZWidgetPos) / (MaxPlayerDistance - MinPlayerDistance) + MinZWidgetPos;
		
		nametag->SetRelativeLocation(FVector(0, 0, heightAdjustment));
	}
}

void AThirdPersonPlayerController::CallMenuEnter()
{
	// Only Player0 can press buttons apparently, so we have to manually handle it for the others
	if (Cast<AInGameGameMode>(UGameplayStatics::GetGameMode(this))->GetIsPaused() && UGameplayStatics::GetPlayerControllerID(this) != 0)
	{
		OnMenuEnter.Broadcast();
	}
}

void AThirdPersonPlayerController::AddNametagWidgetForPlayer(UWidgetComponent* nametagWidget)
{
	otherPlayerNametags.AddUnique(nametagWidget);
}