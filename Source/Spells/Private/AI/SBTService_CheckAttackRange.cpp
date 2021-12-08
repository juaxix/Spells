// Spells - xixgames - juaxix - 2021/2022

#include "AI/SBTService_CheckAttackRange.h"

#include "AI/SAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

namespace
{
	constexpr float MinDistanceInRangeSquared = 2000.0f * 2000.0f;
}

void USBTService_CheckAttackRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	// Check distance between AI Pawn and Followed Actor
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (ensure(BlackboardComponent))
	{
		if (AActor* MoveToActor = Cast<AActor>(BlackboardComponent->GetValueAsObject(SpellsAIController::STARGET_ACTOR_BB_KEY)))
		{
			if (ASAIController* AIController = Cast<ASAIController>(OwnerComp.GetAIOwner()))
			{
				APawn* AIPawn = AIController->GetPawn();
				check(AIPawn);
				const float DistSquared = FVector::DistSquared(MoveToActor->GetActorLocation(), AIPawn->GetActorLocation());
				const bool bWithinRange = DistSquared < MinDistanceInRangeSquared;
				const bool bGoalInSight = bWithinRange ? AIController->LineOfSightTo(MoveToActor) : false;
				BlackboardComponent->SetValueAsBool(AttackRangeKey.IsNone() ?
					SpellsAIController::SWITHIN_ATTACK_SIGHT_BB_KEY:
					AttackRangeKey.SelectedKeyName, bWithinRange);
				BlackboardComponent->SetValueAsBool(AttackSightKey.IsNone() ?
					SpellsAIController::SWITHIN_ATTACK_SIGHT_BB_KEY:
					AttackSightKey.SelectedKeyName, bGoalInSight);
			}
		}
	}
}
