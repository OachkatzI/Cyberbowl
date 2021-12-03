#pragma once

#include "Character/Abilities/AbilityUtils.h"
#include "Engine/World.h"
#include "Components/PrimitiveComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "TimerManager.h"

bool UAbilityUtils::FindTargetPoint(UWorld* worldContext, FVector& targetPoint, FVector traceStart, FVector traceEnd, float wallOffset)
{
	FHitResult hitResult;
	worldContext->LineTraceSingleByProfile(hitResult, traceStart, traceEnd, "AbilityTrace");

	bool foundValidTarget = true;
	FVector floorTraceStart;
	if (hitResult.bBlockingHit)
	{
		if (hitResult.GetComponent()->GetCollisionProfileName() == "StadiumWall")
		{
			//get a point a certain distant away from the wall
			floorTraceStart = hitResult.ImpactPoint + hitResult.ImpactNormal * wallOffset;
		}
		else
		{
			UE_LOG(LogActor, Error, TEXT("AbilityTargeting: Error when tracing for target location."));
			return false;
			//return;
		}
	}
	else
	{
		floorTraceStart = traceEnd;
	}

	FHitResult floorTrace;
	worldContext->LineTraceSingleByProfile(floorTrace, floorTraceStart, floorTraceStart + FVector::DownVector * 10000.f, "AbilityTrace");

	if (floorTrace.bBlockingHit)
	{
		targetPoint = floorTrace.ImpactPoint;
	}
	else
	{
		UE_LOG(LogActor, Error, TEXT("AbilityTargeting: Error when tracing for target location."));
		return false;
	}

	return true;
}

void UAbilityUtils::SpawnTimedEffect(UWorld* worldContext, AActor* attachActor, UNiagaraSystem* effect, float duration,
	FVector location)
{
	if(location == FVector::ZeroVector)
	{
		location = attachActor->GetActorLocation();
	}
	
	UNiagaraComponent* niagaraCmp = UNiagaraFunctionLibrary::SpawnSystemAttached(effect, attachActor->GetRootComponent(), NAME_None, location, FRotator::ZeroRotator, FVector(1, 1, 1), EAttachLocation::KeepWorldPosition, false, ENCPoolMethod::None);

	TWeakObjectPtr<UNiagaraComponent> niagaraCmpWeakPtr(niagaraCmp);


	FTimerDelegate callback;
	callback.BindLambda([niagaraCmpWeakPtr]
	{
		if (niagaraCmpWeakPtr.IsValid())
		{
			//Todo: Here exception thrown when going from tutorial to menu.
			niagaraCmpWeakPtr.Get()->DestroyComponent();
		}
		else
		{
			UE_LOG(LogActor, Error, TEXT("AbilityUtils: Something went wrong when destroying a timed effect (Ptr to NiagaraComponent is null)"));
		}
	});

	FTimerHandle timer;
	worldContext->GetTimerManager().SetTimer(timer, callback, duration, false);
}
