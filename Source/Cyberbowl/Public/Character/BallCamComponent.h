// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BallCamComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CYBERBOWL_API UBallCamComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UBallCamComponent();

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void ToggleBallCam();
	
	UFUNCTION(BlueprintCallable)
	void DoNotFollow();

protected:
	UFUNCTION(BlueprintCallable)
	void FocusBall(float deltaTime);

	UPROPERTY(BlueprintReadOnly)
	class USpringArmComponent* CameraBoom;

	UPROPERTY(BlueprintReadOnly)
	UStaticMeshComponent* PlayBall;

	UPROPERTY(BlueprintReadOnly)
	bool bShouldFollowBall;

};
