// Spells - xixgames - juaxix - 2021/2022

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "SBTTaskNode_RangeAttack.generated.h"

/**
 * 
 */
UCLASS()
class SPELLS_API USBTTaskNode_RangeAttack : public UBTTaskNode
{
	GENERATED_BODY()

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere, Category = "Attack", BlueprintReadOnly)
	TSubclassOf<AActor> ProjectileClass;
};
