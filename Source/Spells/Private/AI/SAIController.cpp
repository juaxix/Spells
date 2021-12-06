// Spells - xixgames - juaxix - 2021/2022

#include "AI/SAIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "Kismet/GameplayStatics.h"

namespace
{
	const FName SMOVE_TO_ACTOR_BB_KEY = TEXT("MoveToActor");
	const FName SMOVE_TO_LOCATION_BB_KEY = TEXT("MoveToLocation");
}

void ASAIController::BeginPlay()
{
	Super::BeginPlay();

	if (DefaultBehaviorTree)
	{
		RunBehaviorTree(DefaultBehaviorTree);
		UBlackboardComponent* BlackboardComponent = GetBlackboardComponent();
		check(BlackboardComponent);
		if (APawn* FirstPlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
		{
			MoveToActorBBKeyID = BlackboardComponent->GetKeyID(SMOVE_TO_ACTOR_BB_KEY);
			if (MoveToActorBBKeyID != FBlackboard::InvalidKey)
			{
				BlackboardComponent->SetValue<UBlackboardKeyType_Object>(MoveToActorBBKeyID, FirstPlayerPawn);
			}

			MoveToLocationBBKeyID = BlackboardComponent->GetKeyID(SMOVE_TO_LOCATION_BB_KEY);
			if (MoveToLocationBBKeyID != FBlackboard::InvalidKey)
			{
				BlackboardComponent->SetValue<UBlackboardKeyType_Vector>(MoveToLocationBBKeyID, FirstPlayerPawn->GetActorLocation());
			}
		}

		
	}
}
