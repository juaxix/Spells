// Spells - xixgames - juaxix - 2021/2022

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "SBTService_CheckAttackRange.generated.h"

/**
 * 
 */
UCLASS()
class SPELLS_API USBTService_CheckAttackRange : public UBTService
{
	GENERATED_BODY()
	
protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, Category = "Spells|AI", meta = (AllowPrivateAccess = "true"))
	FBlackboardKeySelector AttackRangeKey;

	UPROPERTY(EditAnywhere, Category = "Spells|AI", meta = (AllowPrivateAccess = "true"))
	FBlackboardKeySelector AttackSightKey;
};
