#pragma once

#include "CoreMinimal.h"
#include "Character/MovementStates/BaseMovementState.h"
#include "JumpState.generated.h"


/**
 *
 */
UCLASS()
class CYBERBOWL_API UJumpState : public UBaseMovementState
{
	GENERATED_BODY()
public:
	UJumpState() {}

	void InitializeState(class UCBCharacterMovementComponent* moveComponent) override;
	void Activate(ECBMovementMode previousMode) override;
	void Deactivate() override;
	void OnTick(float DeltaTime) override;

protected:
};