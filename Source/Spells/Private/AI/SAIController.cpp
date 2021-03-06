// Spells - xixgames - juaxix - 2021/2022

#include "AI/SAIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "Kismet/GameplayStatics.h"

void ASAIController::BeginPlay()
{
	Super::BeginPlay();

	// cache blackboard key+vars
	if (ensureMsgf(DefaultBehaviorTree, TEXT("Behavior Tree is not set; Please assign it for this AI Controller %s"), *GetName()))
	{
		RunBehaviorTree(DefaultBehaviorTree);
		const UBlackboardComponent* BlackboardComponent = GetBlackboardComponent();
		MoveToActorBBKeyID = BlackboardComponent->GetKeyID(SpellsAIController::STARGET_ACTOR_BB_KEY);
		MoveToLocationBBKeyID = BlackboardComponent->GetKeyID(SpellsAIController::STARGET_LOCATION_BB_KEY);
	}
}

bool ASAIController::SetCurrentTargetActor(AActor* InActor)
{
	if (!IsValid(InActor))
	{
		return false;
	}

	if (UBlackboardComponent* BlackboardComponent = GetBlackboardComponent())
	{
		if (MoveToActorBBKeyID != FBlackboard::InvalidKey)
		{
			if (BlackboardComponent->GetValue<UBlackboardKeyType_Object>(MoveToActorBBKeyID) == InActor)
			{
				return true;
			}

			return BlackboardComponent->SetValue<UBlackboardKeyType_Object>(MoveToActorBBKeyID, InActor);
		}
	}

	return false;
}

AActor* ASAIController::GetCurrentTargetActor() const
{
	if (const UBlackboardComponent* BlackboardComponent = GetBlackboardComponent())
	{
		if (MoveToActorBBKeyID != FBlackboard::InvalidKey)
		{
			return Cast<AActor>(BlackboardComponent->GetValue<UBlackboardKeyType_Object>(MoveToActorBBKeyID));
		}
	}

	return nullptr;
}

bool ASAIController::SetCurrentTargetLocation(const FVector& InLocation)
{
	if (UBlackboardComponent* BlackboardComponent = GetBlackboardComponent())
	{
		if (MoveToLocationBBKeyID != FBlackboard::InvalidKey)
		{
			return BlackboardComponent->SetValue<UBlackboardKeyType_Vector>(MoveToLocationBBKeyID, InLocation);
		}
	}

	return false;
}

FVector ASAIController::GetCurrentTargetLocation() const
{
	if (const UBlackboardComponent* BlackboardComponent = GetBlackboardComponent())
	{
		if (MoveToLocationBBKeyID != FBlackboard::InvalidKey)
		{
			return BlackboardComponent->GetValue<UBlackboardKeyType_Vector>(MoveToLocationBBKeyID);
		}
	}

	return FVector::ZeroVector;
}
