#pragma once

#include "CoreMinimal.h"
#include "Character/MovementStates/BaseMovementState.h"
#include "DoubleJumpState.generated.h"


/**
 *
 */
UCLASS()
class CYBERBOWL_API UDoubleJumpState : public UBaseMovementState
{
	GENERATED_BODY()
public:
	UDoubleJumpState() {}

	void InitializeState(class UCBCharacterMovementComponent* moveComponent) override;
	void Activate(ECBMovementMode previousMode) override;
	void Deactivate() override;
	void OnTick(float DeltaTime) override;

protected:
	void EndDoubleJumpingAnimation();

	UPROPERTY()
	FTimerHandle EndDoubleJumpAnimation;
};