#include "Character/MovementStates/DoubleJumpState.h"
#include "Character/CyberbowlCharacterAnimInstance.h"
#include "Engine/World.h"
#include "TimerManager.h"

void UDoubleJumpState::InitializeState(UCBCharacterMovementComponent* moveComponent)
{
	UBaseMovementState::InitializeState(moveComponent);
}

void UDoubleJumpState::Activate(ECBMovementMode previousMode)
{
	UBaseMovementState::Activate(previousMode);
	if(previousMode != ECBMovementMode::CBMOVE_Dash)
	{
		MovementComponent->animinstance->SetIsDoubleJumping(true);
	}
	MovementComponent->GetWorld()->GetTimerManager().SetTimer(EndDoubleJumpAnimation, this, &UDoubleJumpState::EndDoubleJumpingAnimation, MovementComponent->DoubleJumpDuration);
}

void UDoubleJumpState::EndDoubleJumpingAnimation()
{
	MovementComponent->animinstance->SetIsDoubleJumping(false);
}

void UDoubleJumpState::Deactivate()
{
	UBaseMovementState::Deactivate();
}

void UDoubleJumpState::OnTick(float DeltaTime)
{
	UBaseMovementState::OnTick(DeltaTime);
}
