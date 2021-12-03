// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Actors/IFreezeable.h"
#include "ILaunchable.h"
#include "PlayBall.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBallBooped, AActor*, instigator, float, force); // uwu ; Just how I like it :3
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBallFrozen); 
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBallUnfrozen); 
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBallHit, FName, collisionProfile, float, velocity); 


UCLASS()
class CYBERBOWL_API APlayBall : public AActor, public IFreezeable, public ILaunchable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlayBall();

	UStaticMeshComponent* BallStaticMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayBall Properties")
	float ScaleModifier = 3.f;
		
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayBall Properties")
	float MaxSpeed = 4000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayBall Properties")
	UNiagaraSystem* CollisionEffect;

	//0.95 = Effect only spawns on almost MaxSpeed Collisions
	//0.1 = Effects even spawns on small bounces
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (UIMin = "0.1", UIMax = "0.95"), Category = "PlayBall Properties")
	float CollisionEffectTriggerForce = 0.5f;

	UPROPERTY(BlueprintAssignable, Category = "PlayBall")
	FOnBallBooped OnBallBooped;
		
	UPROPERTY(BlueprintAssignable, Category = "PlayBall")
	FOnBallFrozen OnBallFrozen;
		
	UPROPERTY(BlueprintAssignable, Category = "PlayBall")
	FOnBallUnfrozen OnBallUnfrozen;
		
	UPROPERTY(BlueprintAssignable, Category = "PlayBall")
	FOnBallHit OnBallHit;
		
	UPROPERTY(BlueprintReadOnly, Category = "PlayBall")
	bool IsBallFrozen;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void ResolveCollision(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY(BlueprintReadWrite, Category = "PlayBall")
	float LastHitTeamColor = 0.f;
	
	FVector StartPosition;
	FVector CachedVelocity;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void PushBall(AActor* instigator, float force, FVector direction);

	UFUNCTION(BlueprintCallable)
	void StopBall();

	UFUNCTION(BlueprintCallable)
	void PlayBall();

	UFUNCTION(BlueprintCallable)
	void ResetBallPosition();

	void HideBall(bool hidden);

	//UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "IFreezable")
	void Freeze_Implementation(AActor* instigtr) override;

	//UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "IFreezable")
	void UnFreeze_Implementation() override;

	void Launch_Implementation(FVector direction, float forceHorizontal, float forceVertical, class UNiagaraSystem* launchEffect, float launchEffectDuration) override;
};
