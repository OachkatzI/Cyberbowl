// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerController/MainMenuPlayerController.h"
#include <string>
#include "Blueprint/UserWidget.h"
#include "Engine/World.h"
#include "GameplayTagsModule.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/WLobby.h"

void AMainMenuPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("PreviousCharacter", IE_Pressed, this, &AMainMenuPlayerController::FPreviousCharacterSelected);
	InputComponent->BindAction("NextCharacter", IE_Pressed, this, &AMainMenuPlayerController::FNextCharacterSelected);
	InputComponent->BindAction("PreviousTeam", IE_Pressed, this, &AMainMenuPlayerController::FPreviousTeamSelected);
	InputComponent->BindAction("NextTeam", IE_Pressed, this, &AMainMenuPlayerController::FNextTeamSelected);
	InputComponent->BindAction("ApplyLobbySelection", IE_Pressed, this, &AMainMenuPlayerController::FSelectionApplied);
	InputComponent->BindAction("JoinLobby", IE_Pressed, this, &AMainMenuPlayerController::FLobbyJoined);
	InputComponent->BindAction("MenuBack", IE_Pressed, this, &AMainMenuPlayerController::PopFromMenuStack);
	InputComponent->BindAction("MenuNavigationDown", IE_Pressed, this, &AMainMenuPlayerController::FMainMenuNavigated);
	InputComponent->BindAction("MenuNavigationUp", IE_Pressed, this, &AMainMenuPlayerController::FMainMenuNavigated);
	InputComponent->BindAction("MenuNavigationRight", IE_Pressed, this, &AMainMenuPlayerController::FMainMenuNavigated);
	InputComponent->BindAction("MenuNavigationLeft", IE_Pressed, this, &AMainMenuPlayerController::FMainMenuNavigated);
}

void AMainMenuPlayerController::BeginPlay()
{
	Super::BeginPlay();

	mainMenuGameMode = Cast<AMainMenuGameMode>(GetWorld()->GetAuthGameMode());
	mainMenuGameMode->IndicesReady.AddDynamic(this, &AMainMenuPlayerController::OnIndexReady);
	SetViewTarget(mainMenuGameMode->cinematicCamera);
}

void AMainMenuPlayerController::OnIndexReady()
{
	auto cameras = mainMenuGameMode->GetCharacterSelectionCams();

	const std::string controllerID = std::to_string(UGameplayStatics::GetPlayerControllerID(this));
	const FString controllerIDString(controllerID.c_str());
	const FName controllerIDName = FName(*controllerIDString);

	for (auto cam : cameras)
	{
		if (cam->ActorHasTag(controllerIDName))
		{
			SetViewTargetWithBlend(cam);
		}
	}

	auto dummies = mainMenuGameMode->GetCharacterPreviewDummies();
	
	for(auto dummy : dummies)
	{
		if (dummy->ActorHasTag(controllerIDName))
		{
			CharacterPreviewDummy = Cast<ACyberbowlCharacter>(dummy);
		}
	}
}

void AMainMenuPlayerController::FPreviousCharacterSelected()
{
	PreviousCharacterSelected.Broadcast();
}

void AMainMenuPlayerController::FNextCharacterSelected()
{
	NextCharacterSelected.Broadcast();
}

void AMainMenuPlayerController::FPreviousTeamSelected()
{
	PreviousTeamSelected.Broadcast();
}

void AMainMenuPlayerController::FNextTeamSelected()
{
	NextTeamSelected.Broadcast();
}

void AMainMenuPlayerController::FSelectionApplied()
{
	SelectionApplied.Broadcast();
}

void AMainMenuPlayerController::FLobbyJoined()
{
	LobbyJoined.Broadcast();
}

void AMainMenuPlayerController::FMainMenuNavigated()
{
	MainMenuNavigated.Broadcast();
}

void AMainMenuPlayerController::PushToMenuStack(UUserWidget* newWidget)
{
	if (widgetStack.Num() != 0)
	{
		widgetStack.Last()->RemoveFromParent();
	}

	newWidget->AddToViewport();
	widgetStack.Add(newWidget);
	
	UKismetSystemLibrary::PrintString(this, FString(std::to_string(widgetStack.Num()).c_str()));
}

void AMainMenuPlayerController::PopFromMenuStack()
{
	if (widgetStack.Num() <= 1)
	{
		return;
	}

	widgetStack.Pop()->RemoveFromParent();
	UKismetSystemLibrary::PrintString(this, FString(std::to_string(widgetStack.Num()).c_str()));
	widgetStack.Last()->AddToViewport();
}

bool AMainMenuPlayerController::GetIsAssigned() const
{
	return bIsAssigned;
}

void AMainMenuPlayerController::SetIsAssigned(bool assigned)
{
	bIsAssigned = assigned;
}

UWLobby* AMainMenuPlayerController::GetLobbyWidget() const
{
	return LobbyWidget;
}

void AMainMenuPlayerController::SetLobbyWidget(UWLobby* lobbyWidget)
{
	this->LobbyWidget = lobbyWidget;
}

UUserWidget* AMainMenuPlayerController::GetLobbyNotJoinedWidget() const
{
	return LobbyNotJoinedWidget;
}

void AMainMenuPlayerController::SetLobbyNotJoinedWidget(UUserWidget* lobbyNotJoinedWidget)
{
	this->LobbyNotJoinedWidget = lobbyNotJoinedWidget;
}

ACyberbowlCharacter* AMainMenuPlayerController::GetCharacterPreviewDummy() const
{
	return CharacterPreviewDummy;
}

void AMainMenuPlayerController::SetCharacterPreviewDummy(ACyberbowlCharacter* characterPreviewDummy)
{
	this->CharacterPreviewDummy = characterPreviewDummy;
}
