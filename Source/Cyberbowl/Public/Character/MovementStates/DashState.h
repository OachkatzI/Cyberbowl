#pragma once

#include "CoreMinimal.h"
#include "Character/MovementStates/BaseMovementState.h"
#include "DashState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUpDash);

UCLASS()
class CYBERBOWL_API UDashState : public UBaseMovementState
{
	GENERATED_BODY()
public:
	UDashState() {}

	void InitializeState(class UCBCharacterMovementComponent* moveComponent) override;
	void Activate(ECBMovementMode previousMode) override;
	void Deactivate() override;
	void OnTick(float DeltaTime) override;

	UPROPERTY(BlueprintAssignable, category = "EventDispatchers")
	FOnUpDash OnUpDash;

protected:
	float DefaultGravityScale;
	float DefaultBrakingFrictionFactor;
	bool bKeepMomentum;
	
	FVector DashDirection;
	ECBMovementMode PreviousMovementMode;
	
	void StopDash();

	UPROPERTY()
	FTimerHandle DashTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="DashParams")
	FVector InitialVelocity;
	
	void BindInputActions() override;

};
