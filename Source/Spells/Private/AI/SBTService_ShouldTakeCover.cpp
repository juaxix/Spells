// Spells - xixgames - juaxix - 2021/2022

#include "AI/SBTService_ShouldTakeCover.h"

// Unreal includes
#include "BehaviorTree/BlackboardComponent.h"

// Spells Game includes
#include "AI/SAICharacter.h"
#include "AI/SAIController.h"
#include "Gameplay/SAttributesComponent.h"

void USBTService_ShouldTakeCover::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	if (ShouldTakeCoverKey.IsNone())
	{
		return;
	}

	if (const ASAICharacter* AICharacter = Cast<ASAICharacter>(IsValid(OwnerComp.GetAIOwner()) ? OwnerComp.GetAIOwner()->GetPawn() : nullptr))
	{
		if (UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent())
		{
			if (USAttributesComponent* AttributesComponent = AICharacter->GetAttributesComponent())
			{
				BlackboardComponent->SetValueAsBool(
					ShouldTakeCoverKey.SelectedKeyName,
					AttributesComponent->GetCurrentHealth() / AttributesComponent->GetMaximumHealth() <= LowHealthFractionToCover
				);
			}
		}
	}
	
}
