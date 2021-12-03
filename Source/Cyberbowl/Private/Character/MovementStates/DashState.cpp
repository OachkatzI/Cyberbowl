#include "Character/MovementStates/DashState.h"
#include "Character/CBCharacterMovementComponent.h"
#include "Character/CyberbowlCharacterAnimInstance.h"
#include "Engine/World.h"
#include "TimerManager.h"


void UDashState::InitializeState(UCBCharacterMovementComponent* moveComponent)
{
	UBaseMovementState::InitializeState(moveComponent);
	DefaultGravityScale = MovementComponent->GravityScale;
	DefaultBrakingFrictionFactor = MovementComponent->BrakingFrictionFactor;
}

void UDashState::Activate(ECBMovementMode previousMode)
{
	UBaseMovementState::Activate(previousMode);

	bKeepMomentum = true;
	float forwardAxisInput = InputComponent->GetAxisValue("MoveForward");
	float rightAxisInput = InputComponent->GetAxisValue("MoveRight");
	
	FVector axisInput = FVector(forwardAxisInput, rightAxisInput, 0.f);
	FRotator viewRotation = MovementComponent->GetPawnOwner()->GetControlRotation();

	//Rotate the inputAxis to match with the View Rotation of the camera
	axisInput = axisInput.RotateAngleAxis(viewRotation.Yaw, FVector::UpVector);
	FRotator axisInputRotation = axisInput.ToOrientationRotator();

	float finalDashDuration = MovementComponent->DashDuration;
	if (axisInput.Size() == 0)
	{
		DashDirection = FVector::UpVector;
		//decrease dash duration, otherwise character chrashes into roof
		finalDashDuration /= 2.f;

		//FIX: This is needed, otherwise character will not dash upwards when standing still on ground
		MovementComponent->DoJump(false);

		OnUpDash.Broadcast();
	}
	else if (MovementComponent->DashMomentumStopRange.Contains(FMath::Abs(viewRotation.Yaw - axisInputRotation.Yaw)))
	{
		bKeepMomentum = false;
		DashDirection = axisInput;
	}
	else
	{
		DashDirection = axisInput;
	}
	
	InitialVelocity = MovementComponent->Velocity;
	ACyberbowlCharacter* character = Cast<ACyberbowlCharacter>(MovementComponent->GetPawnOwner());
	if (InitialVelocity.X + InitialVelocity.Y == 0.0)
	{
		character->DashRotationHand = character->DashRotationHandUpwards;
		character->DashRotationFoot = character->DashRotationFootUpwards;
	}
	else
	{
		character->DashRotationHand = character->DashRotationHandNormal;
		character->DashRotationFoot = character->DashRotationFootNormal;
	}
	PreviousMovementMode = previousMode;
	MovementComponent->GravityScale = 0.f;
	MovementComponent->BrakingFrictionFactor = 0.f;
	MovementComponent->StopMovementImmediately();
	MovementComponent->GetWorld()->GetTimerManager().SetTimer(DashTimerHandle, this, &UDashState::StopDash, finalDashDuration);
	MovementComponent->animinstance->SetIsDashing(true);
}

void UDashState::Deactivate()
{
	UBaseMovementState::Deactivate();

	MovementComponent->GravityScale = DefaultGravityScale;
	MovementComponent->BrakingFrictionFactor = DefaultBrakingFrictionFactor;
	MovementComponent->GetWorld()->GetTimerManager().ClearTimer(DashTimerHandle);
	DashDirection = FVector::ZeroVector;
	PreviousMovementMode = ECBMovementMode::CBMOVE_Running;
	MovementComponent->animinstance->SetIsDashing(false);
}

void UDashState::OnTick(float DeltaTime)
{
	MovementComponent->Velocity = DashDirection * MovementComponent->DashForce;
}

void UDashState::StopDash()
{
	FVector newVelocity = MovementComponent->Velocity.GetSafeNormal() * MovementComponent->MaxWalkSpeed;
	MovementComponent->StopMovementImmediately();
	if(bKeepMomentum)
	{
		MovementComponent->Velocity = newVelocity;
	}

	//Bugfix: Dashing out of a wallrun should not switch the character back to a wallrunstate after ending dash
	MovementComponent->SetCBMovementMode(PreviousMovementMode == ECBMovementMode::CBMOVE_Wallrun ? ECBMovementMode::CBMOVE_Jump : PreviousMovementMode);
}

void UDashState::BindInputActions()
{
	//InputComponent->BindAction("Dash", IE_Pressed, this, &UDashState::Dash);
}
