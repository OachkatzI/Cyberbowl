// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModesAndInstances/MainMenuGameMode.h"

#include <string>

#include "Camera/CameraActor.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerController/MainMenuPlayerController.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"
#include "FMODStudioModule.h"
#include "PlayerController/MainMenuPlayerController.h"
#include "Widgets/WMainMenu.h"
#include "TimerManager.h"
#include "GameModesAndInstances/CyberbowlGameInstance.h"

AMainMenuGameMode::AMainMenuGameMode(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DefaultPawnClass = nullptr;
	PrimaryActorTick.bCanEverTick = true;
	PlayerControllerClass = AMainMenuPlayerController::StaticClass();

	IFMODStudioModule::Get();
	
	static ConstructorHelpers::FClassFinder<APawn> DummyCharacterFinder(TEXT("/Game/Characters/Dummy/DummyBase"));
	dummyClass = DummyCharacterFinder.Class;

	static ConstructorHelpers::FClassFinder<UUserWidget> countdownWidgetClassFinder(TEXT("/Game/UI/MainMenu/W_CharacterSelectionComplete"));
	countdownWidgetClass = countdownWidgetClassFinder.Class;

	static ConstructorHelpers::FClassFinder<UUserWidget> lobbyNotJoinedWidgetClassFinder(TEXT("/Game/UI/MainMenu/W_LobbyNotJoined"));
	lobbyNotJoinedWidgetClass = lobbyNotJoinedWidgetClassFinder.Class;

	static ConstructorHelpers::FClassFinder<UUserWidget> lobbyWidgetClassFinder(TEXT("/Game/UI/MainMenu/W_Lobby"));
	lobbyWidgetClass = lobbyWidgetClassFinder.Class;

	static ConstructorHelpers::FClassFinder<UWMainMenu> mainMenuWidgetClassFinder(TEXT("/Game/UI/MainMenu/W_MainMenu"));
	mainMenuWidgetClass = mainMenuWidgetClassFinder.Class;
}

void AMainMenuGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	CreateMainMenu();
	
	SetupCharacterSelection();

	MainMenuWidget->LobbyEntered.AddDynamic(this, &AMainMenuGameMode::OnLobbyEntered);
}

void AMainMenuGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (bAllReady)
	{	
		currentCooldown -= DeltaTime;
	}
}

void AMainMenuGameMode::OnLobbyEntered()
{
	CreatePlayers();

	for (auto controller : mainMenuPlayerControllers)
	{
		auto mainMenuPlayerController = Cast<AMainMenuPlayerController>(controller);
		mainMenuPlayerController->GetLobbyWidget()->PlayerReadyUnready.AddDynamic(this, &AMainMenuGameMode::OnPlayerReadyUnready);
	}
}

void AMainMenuGameMode::OnPlayerReadyUnready()
{
	if (TotalPlayers != PlayersReady)
	{
		return;
	}

	bAllReady = true;

	auto countdownWidget = CreateWidget(UGameplayStatics::GetPlayerControllerFromID(this, 0), countdownWidgetClass);
	countdownWidget->AddToViewport();

	GetWorld()->GetTimerManager().SetTimer(CountdownTimer, this, &AMainMenuGameMode::StartMatch , 1.f, false);
}

void AMainMenuGameMode::StartMatch()
{
	Cast<UCyberbowlGameInstance>(GetGameInstance())->TotalPlayers = TotalPlayers;
	DeleteUnassignedPlayers();
	UGameplayStatics::OpenLevel(this, FName(TEXT("CyberbowlArenaMap")));
}

void AMainMenuGameMode::CreateMainMenu()
{
	auto mainMenuControllerPlayer1 = Cast<AMainMenuPlayerController>(UGameplayStatics::GetPlayerControllerFromID(this, 0));
	
	// Main Menu
	MainMenuWidget = Cast<UWMainMenu>(CreateWidget(mainMenuControllerPlayer1, mainMenuWidgetClass));

	mainMenuControllerPlayer1->PushToMenuStack(MainMenuWidget);
	FInputModeGameAndUI inputMode;
	inputMode.SetWidgetToFocus(MainMenuWidget->GetSlateWidgetFromName(FName(TEXT("Button_Start"))));
	mainMenuControllerPlayer1->SetInputMode(inputMode);
	mainMenuControllerPlayer1->bShowMouseCursor = false;
}

void AMainMenuGameMode::SetupCharacterSelection()
{
	UGameplayStatics::GetAllActorsOfClass(this, dummyClass, CharacterPreviewDummies);

	UGameplayStatics::GetAllActorsOfClassWithTag(this, ACameraActor::StaticClass(), FName(TEXT("SelectionCam")), CharacterSelectionCams);
}

void AMainMenuGameMode::CreatePlayers()
{
	for (int i = 1; i <=3; i++)
	{
		UGameplayStatics::CreatePlayer(this, i, true);
	}

	UGameplayStatics::GetAllActorsOfClass(this, AMainMenuPlayerController::StaticClass(), mainMenuPlayerControllers);
	
	for (auto controller : mainMenuPlayerControllers)
	{
		auto mainMenuController = Cast<AMainMenuPlayerController>(controller);
		
		auto lobbyNotJoinedWidget = CreateWidget(mainMenuController, lobbyNotJoinedWidgetClass);
		mainMenuController->SetLobbyNotJoinedWidget(lobbyNotJoinedWidget);
		lobbyNotJoinedWidget->AddToPlayerScreen();

		auto lobbyWidget = Cast<UWLobby>(CreateWidget(mainMenuController, lobbyWidgetClass));
		mainMenuController->SetLobbyWidget(lobbyWidget);
	}

	IndicesReady.Broadcast();
}

void AMainMenuGameMode::DeleteUnassignedPlayers()
{
	for (auto controller : mainMenuPlayerControllers)
	{
		auto mainMenuController = Cast<AMainMenuPlayerController>(controller);
		if (!mainMenuController->GetIsAssigned())
		{
			mainMenuController->GetLobbyNotJoinedWidget()->RemoveFromParent();
			UGameplayStatics::RemovePlayer(mainMenuController, true);
		}
	}
}

TArray<AActor*> AMainMenuGameMode::GetCharacterPreviewDummies() const
{
	return CharacterPreviewDummies;
}

TArray<AActor*> AMainMenuGameMode::GetCharacterSelectionCams() const
{
	return CharacterSelectionCams;
}
