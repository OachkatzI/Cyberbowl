// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/InputComponent.h"
#include "CBCharacterMovementComponent.h"
#include "NiagaraComponent.h"
#include "BoopComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBoopStarted);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CYBERBOWL_API UBoopComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBoopComponent();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boop Params")
	float Force = 5000.f;

	//Defines how much the ball is pushed upwards when the character is on the ground running
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boop Params")
	float UpwardsAngle = 20.f;
	
	//Time Input (x-Axis) defines the distance between boop center and ball - needs to be in range from 0 to 1
	//Value Output (y-Axis) defines the modifier that is applied to the overall Force - needs to be in range from 0 to 1
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boop Params"/*, meta = (UIMin = "0.1", UIMax = "1.0")*/)
	class UCurveFloat* ForceFalloffCurve;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boop Params")
	float BoopDuration = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boop Params")
	float BoopCooldown = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boop Params")
	UNiagaraSystem* BoopEffect;


	UPROPERTY(BlueprintAssignable, Category = "BoopComponent")
	FOnBoopStarted OnBoopStarted;

	UFUNCTION(BlueprintCallable, Category = "BoopComponent")
	void StartBoop();

protected:
	UInputComponent* InputComponent;
	APlayerController* PlayerController;
	UCBCharacterMovementComponent* MovementComponent;
	USceneComponent* BoopEffectSpawnLocation;
	USpringArmComponent* BoopSpringarm;
	//class APlayerCameraManager* CameraManager;
	
	class UBoxComponent* BoopHitbox;
	FVector BoopHitboxInitialLocation;

	UPROPERTY()
	FTimerHandle BoopDurationHandle;

	UPROPERTY()
	FTimerHandle BoopCooldownHandle;

	UPROPERTY(BlueprintReadOnly)
	bool bBoopActive = false;

	UPROPERTY(BlueprintReadOnly)
	UNiagaraComponent* BoopNiagaraComponent;
	
	virtual void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void EndBoop();
	
	UFUNCTION()
	void PushBall(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void DeactivateBoopHitbox();

	UFUNCTION(BlueprintCallable, Category = "BoopComponent")
	void SpawnBoopEffect();
	
	void AdjustBoopTransforms(float DeltaTime);

	void OnBoopCooldown();
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
