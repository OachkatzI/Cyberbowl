// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "NiagaraSystem.h"
#include "Firewall.generated.h"

UCLASS()
class CYBERBOWL_API AFirewall : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	//AFirewall();
	AFirewall();
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	//UStaticMesh* mesh;


public:	
	// Called every frame
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* box;
	UPROPERTY(VisibleAnywhere)
	USceneComponent* root;

	//FVector firewallColliderSize = (FVector(3000, 1000, 1000));

	
	virtual void Tick(float DeltaTime) override;
	void SetBoxExtent(FVector extent) { box->SetBoxExtent(extent); };
};
