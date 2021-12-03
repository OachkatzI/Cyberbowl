// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Actors/IFreezeable.h"
#include "Actors/ILaunchable.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Character/Abilities/AbilityBase.h"
#include "PlayerController/FPlayerInfo.h"
#include "NiagaraSystem.h"
#include "CyberbowlCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCallErrorFeedback);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBallCamToggled);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnJump);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDoubleJump);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDash);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnVerticalDash);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBoop);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFinishedInitializingWallrun);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterFrozen);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterUnfrozen);

UCLASS(config=Game)
class ACyberbowlCharacter : public ACharacter, public IFreezeable, public ILaunchable
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

public:
	ACyberbowlCharacter();
	ACyberbowlCharacter(const class FObjectInitializer& ObjectInitializer);

	//Custom Cyberbowl Character MovementComponent
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Abilities)
	class UCBCharacterMovementComponent* CBCharacterMoveComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Abilities)
	class UBoopComponent* BoopComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Abilities)
	class UAbilityBase* AbilityComponent;
	
	UFUNCTION()
	void CallMenuEnter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera)
	float TurnRateModifier = 1.f;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera)
	float LookUpRateModifier = 1.f;

	UPROPERTY(BlueprintReadWrite)
	bool bTurretMode;

	UPROPERTY(BlueprintReadOnly)
	bool bIsTargetingAbility;

	UPROPERTY()
	float DefaultGravityScale;
	
	UPROPERTY()
	float DefaultTimeDilation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash Params")
	UNiagaraSystem* DashEffect;

	UPROPERTY(BlueprintReadOnly, Category = "Dash Params")
	FRotator DashRotationHand;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash Params")
	FRotator DashRotationHandNormal = FRotator(0.f, 0.f, 0.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash Params")
	FRotator DashRotationHandUpwards = FRotator(0.f, -45.f, 0.f);

	UPROPERTY(BlueprintReadOnly, Category = "Dash Params")
	FRotator DashRotationFoot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash Params")
	FRotator DashRotationFootNormal = FRotator(180.f, 0.f, 0.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash Params")
	FRotator DashRotationFootUpwards = FRotator(0.f, 0.f, 0.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wallrun Params")
	UNiagaraSystem* WallRunEffect;

#pragma region EventDispatchers
	UPROPERTY(BlueprintAssignable, category = "EventDispatchers")
	FOnBallCamToggled OnToggledBallCam;

	UPROPERTY(BlueprintAssignable, category = "EventDispatchers")
	FOnJump OnJump;

	UPROPERTY(BlueprintAssignable, category = "EventDispatchers")
	FOnDoubleJump OnDoubleJump;

	UPROPERTY(BlueprintAssignable, category = "EventDispatchers")
	FOnDash OnDash;

	UPROPERTY(BlueprintAssignable, category = "EventDispatchers")
	FOnVerticalDash OnVerticalDash;

	UPROPERTY(BlueprintAssignable, category = "EventDispatchers")
	FOnFinishedInitializingWallrun OnFinishedInitializingWallrun;

	UPROPERTY(BlueprintAssignable, category = "EventDispatchers")
	FOnCallErrorFeedback forceFeedback;
	
	UPROPERTY(BlueprintAssignable, category = "EventDispatchers")
	FOnCharacterFrozen OnCharacterFrozen;
	
	UPROPERTY(BlueprintAssignable, category = "EventDispatchers")
	FOnCharacterUnfrozen OnCharacterUnfrozen;

#pragma endregion
	


#pragma region Movement/Camera/Abilities
protected:
	
	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);
	
	void Jump() override;
	
	void Dash();

	void Boop();

	void AbilityTargeting();
	
	void AbilityFire();

	void AbilityCanceled();

	UFUNCTION()
	void CallOnBallCamToggled();

	UFUNCTION()
	void CallOnVerticalDash();

#pragma endregion 
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface


public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

#pragma region Interfaces
	
	void Freeze_Implementation(AActor* instigtr) override;

	void UnFreeze_Implementation() override;

	void Launch_Implementation(FVector direction, float forceHorizontal, float forceVertical, class UNiagaraSystem* launchEffect, float launchEffectDuration) override;

#pragma endregion 
	
	void BeginPlay() override;
	//UFUNCTION(BlueprintCallable, Category = "CyberbowlCharacter")
	//void ToggleAbilities(bool enable);


	// Please don't use this method without supervision, thank you
	// If you do, I'll find you
	void TutorialNameTagSetup(int team, ECBCharacterType characterType);
	
};

