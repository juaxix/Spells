// Spells - xixgames - juaxix - 2021/2022

#pragma once

#include "CoreMinimal.h"
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

	UFUNCTION(BlueprintPure, Category  = "Spells|Action Effects")
	float GetRemainingTime() const { return (TimeStarted + Duration) - GetWorld()->TimeSeconds; }

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "Spells|Action Effects")
	void OnApplyPeriodicEffect(AActor* Instigator);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spells|Action Effects", meta = (AllowPrivateAccess = true))
	float Duration = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spells|Action Effects", meta = (AllowPrivateAccess = true))
	float Period = 0.0f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Spells|Action Effects", meta = (AllowPrivateAccess = true))
	float TimeStarted = 0.0f;

	FTimerHandle DurationHandle;
	FTimerHandle PeriodHandle;
};
