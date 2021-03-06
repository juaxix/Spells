// Spells - xixgames - juaxix - 2021/2022

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SAnimInstance.generated.h"

class USActionsComponent;

UCLASS()
class SPELLS_API USAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:
	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Spells|Action")
	USActionsComponent* ActionsComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spells|Animation")
	bool bIsStunned = false;


};
