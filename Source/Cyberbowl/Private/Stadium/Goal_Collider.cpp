// Fill out your copyright notice in the Description page of Project Settings.

#include "Stadium/Goal_Collider.h"


#include "Actors/PlayBall.h"
#include "Kismet/GameplayStatics.h"

AGoal_Collider::AGoal_Collider()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AGoal_Collider::BeginPlay()
{
	Super::BeginPlay();
	
	BoxComponent = FindComponentByClass<UBoxComponent>();
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AGoal_Collider::OnBeginOverlap);

	Ball = UGameplayStatics::GetActorOfClass(this, APlayBall::StaticClass());
}

void AGoal_Collider::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == Ball)
	{
		OnGoalScored.Broadcast(TeamIndex);
	}
}
