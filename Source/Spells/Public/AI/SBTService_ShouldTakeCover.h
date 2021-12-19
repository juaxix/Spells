// Spells - xixgames - juaxix - 2021/2022

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "SBTService_ShouldTakeCover.generated.h"

UCLASS()
class SPELLS_API USBTService_ShouldTakeCover : public UBTService
{
	GENERATED_BODY()

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, Category = "Spells|AI", meta = (AllowPrivateAccess = "true"))
	FBlackboardKeySelector ShouldTakeCoverKey;

	UPROPERTY(EditAnywhere, Category = "Spells|AI", meta = (AllowPrivateAccess = "true", ClampMin = 0.01f, ClampMax = 1.0f, UIMin = 0.01f, UIMax = 1.0f))
	float LowHealthFractionToCover = 0.3f;
};
