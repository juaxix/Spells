// Spells - xixgames - juaxix - 2021/2022

#include "Gameplay/Actions/SActionEffect.h"

namespace
{
	const FName StopMethodName = TEXT("StopAction");
	const FName PeriodicMethodName = TEXT("OnApplyPeriodicEffect");
}

void USActionEffect::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

	// start timers
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	if (Duration > 0.0f)
	{
		FTimerDelegate DurationDelegate;
		DurationDelegate.BindUFunction(this, StopMethodName, Instigator);
		TimerManager.SetTimer(DurationHandle, DurationDelegate, Duration, false);
	}

	if (Period > 0.0f)
	{
		FTimerDelegate PeriodDelegate;
		PeriodDelegate.BindUFunction(this, PeriodicMethodName, Instigator);
		TimerManager.SetTimer(PeriodHandle, PeriodDelegate, Period, true);
	}
}

void USActionEffect::StopAction_Implementation(AActor* Instigator)
{
	if (!bIsActive)
	{
		return;
	}

	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	// check for next period hit time for a last call (work around tags and so on)
	if (TimerManager.GetTimerRemaining(PeriodHandle) < KINDA_SMALL_NUMBER)
	{
		OnApplyPeriodicEffect_Implementation(Instigator);
	}
	
	Super::StopAction_Implementation(Instigator);

	// stop timers
	TimerManager.ClearTimer(PeriodHandle);
	TimerManager.ClearTimer(DurationHandle);

	if (USActionsComponent* OwningComponent = GetOwningComponent())
	{
		OwningComponent->RemoveAction(this);
	}
}

void USActionEffect::OnApplyPeriodicEffect_Implementation(AActor* Instigator)
{

}