// Spells - xixgames - juaxix - 2021/2022

#pragma once

// Unreal includes
#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"

// Spells includes
#include "Gameplay/Actions/SAction.h"

#include "SActionEffect.generated.h"

/**
 * Effects produced by actions, like Buffs
 */
UCLASS()
class SPELLS_API USActionEffect : public USAction
{
	GENERATED_BODY()

public:
	USActionEffect()
	{
		bAutoStart = true;
	}

	virtual void StartAction_Implementation(AActor* Instigator) override;

	virtual void StopAction_Implementation(AActor* Instigator) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override
	{
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);

		DOREPLIFETIME(USActionEffect, TimeStarted);
	}

	UFUNCTION(BlueprintPure, Category  = "Spells|Action Effects")
	float GetRemainingTime() const
	{
		const UWorld* World = GetWorld();
		const AGameStateBase* GameState = World->GetGameState();
		const float TimeSeconds = IsValid(GameState)
					? GameState->GetServerWorldTimeSeconds()
					: World->TimeSeconds;

		return (TimeStarted + Duration) - TimeSeconds;
	}

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "Spells|Action Effects")
	void OnApplyPeriodicEffect(AActor* Instigator);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spells|Action Effects", meta = (AllowPrivateAccess = true))
	float Duration = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spells|Action Effects", meta = (AllowPrivateAccess = true))
	float Period = 0.0f;

	UPROPERTY(Replicated, VisibleInstanceOnly, BlueprintReadOnly, Category = "Spells|Action Effects", meta = (AllowPrivateAccess = true))
	float TimeStarted = 0.0f;

	FTimerHandle DurationHandle;
	FTimerHandle PeriodHandle;
};
