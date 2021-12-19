// Spells - xixgames - juaxix - 2021/2022


#include "AI/SBTTaskNode_RegenerateAttribute.h"

#include "AI/SAICharacter.h"
#include "AI/SAIController.h"

EBTNodeResult::Type USBTTaskNode_RegenerateAttribute::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (const ASAIController* AIController = Cast<ASAIController>(OwnerComp.GetAIOwner()))
	{
		if (ASAICharacter* AICharacter = Cast<ASAICharacter>(AIController->GetPawn()))
		{
			switch (Attribute)
			{
				default: 
					return EBTNodeResult::Failed;
				case EAttributesTypes::HEALTH:
					AICharacter->GetAttributesComponent()->ApplyHealthChange(
						Value, 
						AICharacter, 
						FHitResult{AICharacter->GetMuzzleLocation(), AICharacter->GetActorLocation()}
					);

					break;
			}

			return EBTNodeResult::Succeeded;
		}
	}

	return EBTNodeResult::Failed;
}
