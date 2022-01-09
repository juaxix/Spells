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
	
protected:
	UFUNCTION(BlueprintNativeEvent, Category = "Spells|Action Effect")
	void OnApplyPeriodicEffect(AActor* Instigator);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spells|Action Effect", meta = (AllowPrivateAccess = true))
	float Duration;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spells|Action Effect", meta = (AllowPrivateAccess = true))
	float Period;
	
	FTimerHandle DurationHandle;
	FTimerHandle PeriodHandle;
};
