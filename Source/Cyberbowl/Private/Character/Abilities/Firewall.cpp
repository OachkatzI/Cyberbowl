// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Abilities/Firewall.h"
#include "DrawDebugHelpers.h"
#include "NiagaraFunctionLibrary.h"

// Sets default values
AFirewall::AFirewall()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	root = CreateDefaultSubobject<USceneComponent>(FName("FirewallRoot"));
	RootComponent = root;
	box = CreateDefaultSubobject<UBoxComponent>(FName("FirewallBox"));
	box->SetupAttachment(root);

}

// Called when the game starts or when spawned
void AFirewall::BeginPlay()
{
	Super::BeginPlay();
	box->SetCollisionProfileName("BlockAllDynamic");
}

// Called every frame
void AFirewall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//DrawDebugBox(GetWorld(), GetActorLocation(), box->GetUnscaledBoxExtent(), GetActorRotation().Quaternion(), FColor::Black, false, 1, 0, 10);
}

