// Spells - xixgames - juaxix - 2021/2022

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Player/SAttributesComponent.h"
#include "SBTTaskNode_RegenerateAttribute.generated.h"

UCLASS()
class SPELLS_API USBTTaskNode_RegenerateAttribute : public UBTTaskNode
{
	GENERATED_BODY()

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	
	UPROPERTY(EditAnywhere, Category = "Spells|Attributes", BlueprintReadOnly)
	EAttributesTypes Attribute;

	UPROPERTY(EditAnywhere, Category = "Spells|Attributes", BlueprintReadOnly)
	float Value = 5.0f;
};
