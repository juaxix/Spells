// Spells - xixgames - juaxix - 2021/2022

#include "AI/SBTTaskNode_RangeAttack.h"

// Unreal includes
#include "BehaviorTree/BlackboardComponent.h"

// Spells game includes
#include "AI/SAICharacter.h"
#include "AI/SAIController.h"
#include "Gameplay/Attacks/SMagicProjectile.h"
#include "Gameplay/SAttributesComponent.h"

EBTNodeResult::Type USBTTaskNode_RangeAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (ASAIController* AIController = Cast<ASAIController>(OwnerComp.GetAIOwner()))
	{
		if (ASAICharacter* AICharacter = Cast<ASAICharacter>(AIController->GetPawn()))
		{
			if (AActor* TargetActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(SpellsAIController::STARGET_ACTOR_BB_KEY)))
			{
				if (USAttributesComponent* Attributes = USAttributesComponent::GetAttributesComponent(TargetActor))
				{
					if (Attributes->IsAlive())
					{
						FVector MuzzleLocation = AICharacter->GetMuzzleLocation();
						FVector Direction = TargetActor->GetActorLocation() - MuzzleLocation;
						FRotator MuzzleRotation = Direction.Rotation();
						MuzzleRotation.Pitch += FMath::RandRange(0.0f, MaxBulletSpread); // 0 to avoid aiming to the ground
						MuzzleRotation.Yaw += FMath::RandRange(-MaxBulletSpread, MaxBulletSpread);
						FActorSpawnParameters SpawnParameters;
						SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
						SpawnParameters.Instigator = AICharacter;
						if (AActor* NewProjectile = GetWorld()->SpawnActor<AActor>(ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParameters))
						{
							if (ASMagicProjectile* MagicProjectile = Cast<ASMagicProjectile>(NewProjectile))
							{
								MagicProjectile->SetProjectileDamage(AICharacter->ProjectileDamage);
								if (AICharacter->bOverrideProjectileEffects)
								{
									MagicProjectile->SetActionEffectClasses(AICharacter->ProjectileActionEffects, false);
								}
							}

							return EBTNodeResult::Succeeded;
						}
					}
				}
			}
		}
	}

	return EBTNodeResult::Failed;
}
