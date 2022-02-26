// Spells - xixgames - juaxix - 2021/2022

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SAIAnimInstance.generated.h"

/**
 * Animation instance used for the AI Characters
 */
UCLASS()
class SPELLS_API USAIAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spells|Animation")
	float Speed = 0.0f;
};
