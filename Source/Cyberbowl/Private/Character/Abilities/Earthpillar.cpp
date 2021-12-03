// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Abilities/Earthpillar.h"

#include "Actors/PlayBall.h"
#include "Character/CyberbowlCharacter.h"
#include "TimerManager.h"
#include "Engine/Engine.h"
#include "Kismet/KismetMathLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"


// Sets default values
AEarthpillar::AEarthpillar()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(FName("EarthPillarRoot"));
	RootComponent = Root;
	PillarMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("PillarMesh"));
	PillarMesh->SetupAttachment(Root);
	PillarMesh->SetCollisionProfileName("BlockAllDynamic");
	TriggerMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("TriggerMesh"));
	TriggerMesh->SetupAttachment(Root);
	TriggerMesh->SetCollisionProfileName("OverlapAllDynamic");
	currPlayerTeam = -1;
	
}


// Called when the game starts or when spawned
void AEarthpillar::BeginPlay()
{
	Super::BeginPlay();
	TriggerMesh->OnComponentBeginOverlap.AddDynamic(this, &AEarthpillar::BeginOverlap);
}

// Called every frame
void AEarthpillar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float val = GetActorLocation().Z;

	if ((GetActorLocation().Z) >= MaxRise)
	{
		bIsRising = false;
		TriggerMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	
	if (bIsRising && GetActorLocation().Z <= MaxRise)
	{
		Rising(DeltaTime);
	}
	else if (val > maxLowering)
	{
		Lowering(DeltaTime);
	}
	else
	{
		TriggerMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}

	//TickLaunch();
}

void AEarthpillar::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor->GetClass()->ImplementsInterface(ULaunchable::StaticClass()) && !GetWorld()->GetTimerManager().IsTimerActive(LaunchTimerHandle))
	{
		GetWorld()->GetTimerManager().SetTimer(LaunchTimerHandle, this, &AEarthpillar::EndLaunch, LaunchCooldown/* - LaunchCooldown / 10.f*/);
		ILaunchable::Execute_Launch(OtherActor, OtherActor->GetVelocity().GetSafeNormal(), LaunchForceHorizontal, LaunchForceVertical, LaunchEffect, LaunchEffectDuration);
		bIsRising = true;
		OnActorLaunched.Broadcast(OtherActor);
	}
}

void AEarthpillar::Rising(float DeltaTime)
{
	SetActorLocation(GetActorLocation() + GetActorUpVector() * FVector(RiseTime * DeltaTime));
}

void AEarthpillar::Lowering(float DeltaTime)
{
	SetActorLocation(GetActorLocation() - GetActorUpVector() * FVector(LoweringTime * DeltaTime));
}

void AEarthpillar::TickLaunch()
{
	//if (!GetWorld()->GetTimerManager().IsTimerActive(LaunchTimerHandle))
	//{
	//	return;
	//}

	//float elapsedLeapTime = GetWorld()->GetTimerManager().GetTimerElapsed(LaunchTimerHandle);
	//elapsedLeapTime /= LaunchCooldown;
	//
	//FVector leapMiddle = LaunchStart + (LaunchTarget - LaunchStart) / 2.f + FVector::UpVector * LaunchHeight;

	//FVector startToMiddle = FMath::Lerp<FVector>(LaunchStart, leapMiddle, elapsedLeapTime);
	//FVector middleToEnd = FMath::Lerp<FVector>(leapMiddle, LaunchTarget, elapsedLeapTime);
	//FVector leapLocation = FMath::Lerp<FVector>(startToMiddle, middleToEnd, elapsedLeapTime);

	//LaunchedActor->SetActorLocation(leapLocation, true);

	//if (!bAdjustRotation)
	//{
	//	FVector start2middle = leapMiddle - LaunchStart;
	//	auto launchAngle = (FVector::UpVector.X * start2middle.X + FVector::UpVector.Y * start2middle.Y + FVector::UpVector.Z * start2middle.Z) / (FVector::UpVector.Size() * start2middle.Size());
	//	auto launchAngleDegr = (launchAngle * 180) / PI;
	//	auto currRotation = GetActorRotation();

	//	//SetActorRotation(FRotator(launchAngle));
	//	SetActorRotation(FRotator(90-launchAngleDegr, currRotation.Yaw, currRotation.Roll));
	//	bAdjustRotation = true;
	//}
}

void AEarthpillar::EndLaunch()
{
	//LaunchedActor = nullptr;
}

void AEarthpillar::InitializePillar(int playerTeam, float maxLoweringPos, float lifeSpan)
{
	currPlayerTeam = playerTeam;
	maxLowering = maxLoweringPos;
	SetLifeSpan(lifeSpan);
	//LaunchTarget = launchTarget;
	//LaunchCooldown = launchDuration;
	//LaunchHeight = launchHeight;
}

float AEarthpillar::GetPillarLocationZ()
{
	auto boundingBox = GetComponentsBoundingBox(true);
	return boundingBox.Min.Z;
}
