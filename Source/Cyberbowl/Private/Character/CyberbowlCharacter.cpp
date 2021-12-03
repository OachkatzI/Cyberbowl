// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "Character/CyberbowlCharacter.h"
#include "Character/Abilities/AirAbility.h"
#include "Camera/CameraComponent.h"
#include "Character/Abilities/AbilityBase.h"
#include "Character/Abilities/AbilityUtils.h"
#include "Character/BallCamComponent.h"
#include "Character/BoopComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Character/CBCharacterMovementComponent.h"
#include "Character/CyberbowlCharacterAnimInstance.h"
#include "Components/Button.h"
#include "PlayerController/ThirdPersonPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Components/WidgetComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "PlayerController/TutorialPlayerController.h"
#include "Widgets/WNameTag.h"

//////////////////////////////////////////////////////////////////////////
// ACyberbowlCharacter

ACyberbowlCharacter::ACyberbowlCharacter()
{

}

ACyberbowlCharacter::ACyberbowlCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCBCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
	//
}

void ACyberbowlCharacter::CallMenuEnter()
{
	auto thirdPersonController = Cast<AThirdPersonPlayerController>(Controller);

	if (thirdPersonController)
	{
		thirdPersonController->CallMenuEnter();
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void ACyberbowlCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("MenuEnter", IE_Pressed, this, &ACyberbowlCharacter::CallMenuEnter);

	PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &ACyberbowlCharacter::Dash);
	PlayerInputComponent->BindAction("Boop", IE_Pressed, this, &ACyberbowlCharacter::Boop);


	PlayerInputComponent->BindAxis("MoveForward", this, &ACyberbowlCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACyberbowlCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ACyberbowlCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ACyberbowlCharacter::LookUpAtRate);

	// Handle ability input
	PlayerInputComponent->BindAction("Ult", IE_Pressed, this, &ACyberbowlCharacter::AbilityTargeting);
	PlayerInputComponent->BindAction("Ult", IE_Released, this, &ACyberbowlCharacter::AbilityFire);
	PlayerInputComponent->BindAction("CancelUlt", IE_Pressed, this, &ACyberbowlCharacter::AbilityCanceled);

	PlayerInputComponent->BindAction("ToggleBallCam", IE_Pressed, this, &ACyberbowlCharacter::CallOnBallCamToggled);
}

void ACyberbowlCharacter::Jump()
{
	if (CBCharacterMoveComponent->GetCBMovementMode() == ECBMovementMode::CBMOVE_DoubleJump
		|| CBCharacterMoveComponent->GetCBMovementMode() == ECBMovementMode::CBMOVE_Wallrun
		|| CBCharacterMoveComponent->GetCBMovementMode() == ECBMovementMode::CBMOVE_Dash)
	{
		return;
	}
	
	Super::Jump();

	if(CBCharacterMoveComponent->GetCBMovementMode() == ECBMovementMode::CBMOVE_Jump)
	{
		CBCharacterMoveComponent->SetCBMovementMode(ECBMovementMode::CBMOVE_DoubleJump);
		OnDoubleJump.Broadcast();
		return;
	}

	CBCharacterMoveComponent->SetCBMovementMode(ECBMovementMode::CBMOVE_Jump);
	OnJump.Broadcast();
}

void ACyberbowlCharacter::Freeze_Implementation(AActor* instigtr)
{
	//Check for friendly fire
	if(auto instigatorAsChar = Cast<ACyberbowlCharacter>(instigtr))
	{
		if (auto instigatorController = Cast<AThirdPersonPlayerController>(instigatorAsChar->Controller))
		{
			auto targetController = Cast<AThirdPersonPlayerController>(this->Controller);
			if(instigatorController->currPlayerTeam == targetController->currPlayerTeam)
			{
				return;
			}
		}
	}

	//Stop Movement & disable Input
	CBCharacterMoveComponent->StopMovementImmediately();
	DefaultGravityScale = CBCharacterMoveComponent->GravityScale;
	CBCharacterMoveComponent->GravityScale = 0.f;
	DisableInput(Cast<APlayerController>(Controller));
	//ToggleAbilities(false);
	
	//Pause all animations
	DefaultTimeDilation = CustomTimeDilation;
	CustomTimeDilation = 0.f;

	OnCharacterFrozen.Broadcast();
}

void ACyberbowlCharacter::UnFreeze_Implementation()
{
	CBCharacterMoveComponent->GravityScale = DefaultGravityScale;
	EnableInput(Cast<APlayerController>(Controller));
	//ToggleAbilities(true);
	CustomTimeDilation = DefaultTimeDilation;
	CBCharacterMoveComponent->SetCBMovementMode(ECBMovementMode::CBMOVE_DoubleJump);
	OnCharacterUnfrozen.Broadcast();
}

void ACyberbowlCharacter::Launch_Implementation(FVector direction, float forceHorizontal, float forceVertical, UNiagaraSystem* launchEffect, float launchEffectDuration)
{
	CBCharacterMoveComponent->StopMovementImmediately();
	CBCharacterMoveComponent->DoJump(true);
	CBCharacterMoveComponent->Velocity = direction * forceHorizontal;
	CBCharacterMoveComponent->Velocity.Z = forceVertical;

	FVector effectSpawnLocation;
	auto feetDistance = GetMesh()->GetSocketLocation("RightFoot") - GetMesh()->GetSocketLocation("LeftFoot");
	effectSpawnLocation = GetMesh()->GetSocketLocation("LeftFoot") + feetDistance / 2.f;
	
	UAbilityUtils::SpawnTimedEffect(GetWorld(), this, launchEffect, launchEffectDuration, effectSpawnLocation);
	
	CBCharacterMoveComponent->SetCBMovementMode(ECBMovementMode::CBMOVE_Jump);
}

void ACyberbowlCharacter::BeginPlay()
{
	Super::BeginPlay();

	BoopComponent = FindComponentByClass<UBoopComponent>();
	CBCharacterMoveComponent = FindComponentByClass<UCBCharacterMovementComponent>();
	AbilityComponent = FindComponentByClass<UAbilityBase>();
	if(!BoopComponent || !CBCharacterMoveComponent || !AbilityComponent)
	{
		UE_LOG(LogActor, Error, TEXT("CyberbowlCharacter: Vital Components not set in character blueprint! (BoopComponent, CBCharacterMovementComponent, AbilityComponent)"));
	}
	{
		CBCharacterMoveComponent->OnVertDash.AddDynamic(this, &ACyberbowlCharacter::CallOnVerticalDash);
	}
}

//DOES NOT deactivate the abilities
//void ACyberbowlCharacter::ToggleAbilities(bool enable)
//{
//	TSet<UActorComponent*> cmps = GetComponents();
//
//	for(auto cmp : cmps)
//	{
//		if(auto boopCmp = Cast<UBoopComponent>(cmp))
//		{
//			boopCmp->SetActive(enable, true);
//		}
//		else if (auto abilityCmp = Cast<UAbilityBase>(cmp))
//		{
//			abilityCmp->SetActive(enable, true);
//		}
//	}
//}

void ACyberbowlCharacter::Dash()
{
	auto cooldownComponent = FindComponentByClass<UCooldownComponent>();
	if (!cooldownComponent)
	{
		UE_LOG(LogActor, Error, TEXT("CyberbowlCharacter: CoolDownComponent not found"));
		return;	
	}
	if(!cooldownComponent->IsDashReady())
	{
		forceFeedback.Broadcast();
	}
	
	if(cooldownComponent->IsDashReady())
	{
		CBCharacterMoveComponent->SetCBMovementMode(ECBMovementMode::CBMOVE_Dash);
		cooldownComponent->StartCooldown("Dash");

		OnDash.Broadcast();
	}
}

void ACyberbowlCharacter::Boop()
{
	if(CBCharacterMoveComponent->GetCBMovementMode() != ECBMovementMode::CBMOVE_Dash)
	{
		BoopComponent->StartBoop();
	}
}

void ACyberbowlCharacter::AbilityTargeting()
{
	auto cooldownComponent = FindComponentByClass<UCooldownComponent>();

	auto abilityState = AbilityComponent->GetAbilityState();
	
	if (cooldownComponent->IsUltReady())
	{
		AbilityComponent->SetAbilityState(EAbilityState::ABILITY_DEFAULT);
		
		if (abilityState == EAbilityState::ABILITY_DEFAULT)
		{
			AbilityComponent->SetAbilityState(EAbilityState::ABILITY_TARGETING);
			bIsTargetingAbility = true;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("CyberbowlCharacter::AbilityTargeting - Something is terribly wrong!"))
		}
	}
	else
	{
		forceFeedback.Broadcast();
	}
}

void ACyberbowlCharacter::AbilityFire()
{
	if (bIsTargetingAbility)
	{
		AbilityComponent->SetAbilityState(EAbilityState::ABILITY_FIRE);
		bIsTargetingAbility = false;
	}
}

void ACyberbowlCharacter::AbilityCanceled()
{
	if(bIsTargetingAbility)
	{
		AbilityComponent->SetAbilityState(EAbilityState::ABILITY_DEFAULT);
		bIsTargetingAbility = false;
		AbilityComponent->ResetTargeting();
	}
}

void ACyberbowlCharacter::CallOnBallCamToggled()
{
	Cast<UBallCamComponent>(GetComponentByClass(UBallCamComponent::StaticClass()))->ToggleBallCam();

	OnToggledBallCam.Broadcast();
}

void ACyberbowlCharacter::CallOnVerticalDash()
{
	OnVerticalDash.Broadcast();
}

void ACyberbowlCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ACyberbowlCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ACyberbowlCharacter::MoveForward(float Value)
{
	//if ((Controller != NULL) && (Value != 0.0f))
	//{
	//	// find out which way is forward
	//	const FRotator Rotation = Controller->GetControlRotation();
	//	const FRotator YawRotation(0, Rotation.Yaw, 0);

	//	// get forward vector
	//	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	//	AddMovementInput(Direction, Value);
	//}
}

void ACyberbowlCharacter::MoveRight(float Value)
{
	//if ( (Controller != NULL) && (Value != 0.0f) )
	//{
	//	// find out which way is right
	//	const FRotator Rotation = Controller->GetControlRotation();
	//	const FRotator YawRotation(0, Rotation.Yaw, 0);
	//
	//	// get right vector 
	//	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	//	// add movement in that direction
	//	AddMovementInput(Direction, Value);
	//}
}

void ACyberbowlCharacter::TutorialNameTagSetup(int team, ECBCharacterType characterType)
{
	TArray<UWidgetComponent*, FDefaultAllocator> widgetComponents;
	GetComponents<UWidgetComponent, FDefaultAllocator>(widgetComponents);

	

	UWidgetComponent* widgetComponent = widgetComponents.Pop();
	widgetComponent->Activate();
	UWNameTag* nameTagWidget = Cast<UWNameTag>(widgetComponent->GetUserWidgetObject());
	UButton* nameplate = Cast<UButton>(nameTagWidget->GetWidgetFromName("TeamColorButton"));
	nameTagWidget->CharacterName = ToCharacterName(characterType);

	if (team == 1)
	{
		nameplate->SetBackgroundColor(FLinearColor(0.9, 0.3, 0, 0.5));
	}
	else
	{
		nameplate->SetBackgroundColor(FLinearColor(0, 0.15, 0.55, 0.5));
	}

	const auto playerController = Cast<ATutorialPlayerController>(UGameplayStatics::GetPlayerControllerFromID(this, 0));
	
	widgetComponent->SetOwnerPlayer(playerController->GetLocalPlayer());
	nameTagWidget->SetOwningPlayer(playerController);
	nameTagWidget->SetOwningLocalPlayer(playerController->GetLocalPlayer());
	nameTagWidget->IsAssigned = true;

	for (auto widgetComp : widgetComponents)
	{
		if (!Cast<UWNameTag>(widgetComp->GetUserWidgetObject())->IsAssigned)
		widgetComp->DestroyComponent();
	}
}
