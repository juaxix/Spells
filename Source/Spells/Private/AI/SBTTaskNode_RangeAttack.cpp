// Spells - xixgames - juaxix - 2021/2022

#include "AI/SBTTaskNode_RangeAttack.h"

#include "AI/SAICharacter.h"
#include "AI/SAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type USBTTaskNode_RangeAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (ASAIController* AIController = Cast<ASAIController>(OwnerComp.GetAIOwner()))
	{
		if (ASAICharacter* AICharacter = Cast<ASAICharacter>(AIController->GetPawn()))
		{
			if (AActor* TargetActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(SpellsAIController::STARGET_ACTOR_BB_KEY)))
			{
				FVector MuzzleLocation = AICharacter->GetMuzzleLocation();
				FVector Direction = TargetActor->GetActorLocation() - MuzzleLocation;
				FRotator MuzzleRotation = Direction.Rotation();
				FActorSpawnParameters SpawnParameters;
				SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
				SpawnParameters.Instigator = AICharacter;
				if (AActor* NewProjectile = GetWorld()->SpawnActor<AActor>(ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParameters))
				{
					return EBTNodeResult::Succeeded;
				}
			}
		}
	}

	return EBTNodeResult::Failed;
}
