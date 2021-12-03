#pragma once
#include "CoreMinimal.h"
#include "Character/MovementStates/IMovementState.h"
#include "Components/InputComponent.h"

#include "BaseMovementState.generated.h"


enum class ECBMovementMode : unsigned char;


UCLASS()
class CYBERBOWL_API UBaseMovementState : public UObject
{
	GENERATED_BODY()
public:
	UBaseMovementState() {};
	virtual void InitializeState(class UCBCharacterMovementComponent* moveComponent);
	virtual void Activate(ECBMovementMode previousMode);
	virtual void Deactivate();
	virtual void OnTick(float DeltaTime);

protected:
	UCBCharacterMovementComponent* MovementComponent;
	UPROPERTY()
	UInputComponent* InputComponent;

	virtual void BindInputActions();
};