// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/PlayBall.h"

#include "Character/Abilities/AbilityUtils.h"
#include "Engine/Engine.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

// Sets default values
APlayBall::APlayBall()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APlayBall::BeginPlay()
{
	Super::BeginPlay();
	
	BallStaticMesh = Cast<UStaticMeshComponent>(GetComponentsByTag(UStaticMeshComponent::StaticClass(), "PhysicalMesh").Last());
	if (!BallStaticMesh)
	{
		return;
	}
	BallStaticMesh->SetWorldScale3D(FVector(ScaleModifier));
	StartPosition = GetActorLocation();

	OnActorHit.AddDynamic(this, &APlayBall::ResolveCollision);

	//enable Gravity and Physics after Countdown endet
	StopBall();
}

void APlayBall::ResolveCollision(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	//TODO: hotfix: this prevents constant firing of the event when the ball just rolls on the floor
	if (NormalImpulse.Size() <= 40000.f)
	{
		return;
	}
	
	FName otherCollisionProfile = Hit.Component->GetCollisionProfileName();
	float ballVelMagnitude = CachedVelocity.Size();
	float ballVelMagnitudeNormalized = ballVelMagnitude / MaxSpeed;
	//GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.f, FColor::Emerald, NormalImpulse.ToString());

	if (ballVelMagnitudeNormalized >= CollisionEffectTriggerForce)
	{
		auto niagraCmp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), CollisionEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation().Add(90.f, 0, 0));
		niagraCmp->SetFloatParameter("User.CurveIndex.Color", LastHitTeamColor);
	}
	OnBallHit.Broadcast(otherCollisionProfile, ballVelMagnitudeNormalized);
}

// Called every frame
void APlayBall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CachedVelocity = GetVelocity();
	if(CachedVelocity.Size() > MaxSpeed)
	{
		BallStaticMesh->SetAllPhysicsLinearVelocity(CachedVelocity.GetClampedToMaxSize(MaxSpeed)) ;
		CachedVelocity = GetVelocity();
	}
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("%s"), *GetVelocity().ToString()));
}

void APlayBall::PushBall(AActor* instigator, float force, FVector direction)
{
	if(IsBallFrozen)
	{
		UnFreeze_Implementation();
	}

	//Is this only moving the mesh? -> Marco: the mesh is a scene component and the root of the actor, so everything in this actor is moved
	BallStaticMesh->AddImpulse(direction * force, NAME_None, true);
	
	OnBallBooped.Broadcast(instigator, force);
}

void APlayBall::PlayBall()
{
	BallStaticMesh->SetEnableGravity(true);
	BallStaticMesh->SetSimulatePhysics(true);
}

void APlayBall::StopBall()
{
	BallStaticMesh->SetEnableGravity(false);
	BallStaticMesh->SetSimulatePhysics(false);
}

void APlayBall::ResetBallPosition()
{
	BallStaticMesh->SetWorldLocation(StartPosition);
	UE_LOG(LogTemp, Warning, TEXT("%s"), *StartPosition.ToString());
}

void APlayBall::Freeze_Implementation(AActor* instigtr)
{
	if(IsBallFrozen)
	{
		return;
	}
	
	IsBallFrozen = true;
	BallStaticMesh->SetSimulatePhysics(false);
	OnBallFrozen.Broadcast();
}

void APlayBall::UnFreeze_Implementation()
{
	if (!IsBallFrozen)
	{
		return;
	}

	IsBallFrozen = false;
	BallStaticMesh->SetSimulatePhysics(true);
	OnBallUnfrozen.Broadcast();
}

void APlayBall::Launch_Implementation(FVector direction, float forceHorizontal, float forceVertical, UNiagaraSystem* launchEffect, float launchEffectDuration)
{
	FVector newVel = direction * forceHorizontal;
	newVel.Z = forceVertical;
	BallStaticMesh->AddImpulse(newVel, NAME_None, true);
	UAbilityUtils::SpawnTimedEffect(GetWorld(), this, launchEffect, launchEffectDuration);
}

void APlayBall::HideBall(bool hidden)
{
	SetActorHiddenInGame(hidden);
}
