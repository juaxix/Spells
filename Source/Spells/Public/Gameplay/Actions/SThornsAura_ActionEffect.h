// Spells - xixgames - juaxix - 2021/2022

#pragma once

// Unreal includes
#include "CoreMinimal.h"

// Spells includes
#include "SActionEffect.h"

#include "SThornsAura_ActionEffect.generated.h"

class USAttributesComponent;

UCLASS()
class SPELLS_API USThornsAura_ActionEffect : public USActionEffect
{
	GENERATED_BODY()

public:
	USThornsAura_ActionEffect()
	{
		Period = 0.0f;
		Duration = 0.0f;
	}

	virtual void StartAction_Implementation(AActor* Instigator) override;

	virtual void StopAction_Implementation(AActor* Instigator) override;

protected:
	UFUNCTION()
	void OnHealthChanged(AActor* InstigatorActor, USAttributesComponent* OwningAttributesComp, float NewHealth, float Delta, const FHitResult& Hit);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spells|Thorns Aura", meta = (AllowPrivateAccess = "true"))
	float DamageFraction = 0.23f;

private:
	UPROPERTY(Transient)
	USkeletalMeshSocket const* SkeletalSocket = nullptr;
};
