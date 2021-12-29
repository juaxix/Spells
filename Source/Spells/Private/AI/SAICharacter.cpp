// Spells - xixgames - juaxix - 2021/2022

#include "AI/SAICharacter.h"

/// Unreal includes
#include "BehaviorTree/BlackboardComponent.h"
#include "BrainComponent.h"
#include "DrawDebugHelpers.h"
#include "Perception/PawnSensingComponent.h"

/// Spells game includes
#include "AI/SAIController.h"
#include "Gameplay/SAttributesComponent.h"

namespace
{
	const FName SMUZZLE_NAME = TEXT("Muzzle_01");
}

ASAICharacter::ASAICharacter()
	: bDebug(false)
{
	PrimaryActorTick.bCanEverTick = false;
	PawnSensingComponent = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComponent"));
	AttributesComponent = CreateDefaultSubobject<USAttributesComponent>(TEXT("AttributesComponent"));
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void ASAICharacter::BeginPlay()
{
	Super::BeginPlay();

	// cache muzzle
	MuzzleSocket = GetMesh()->GetSocketByName(SMUZZLE_NAME);

	// bind events
	PawnSensingComponent->OnSeePawn.AddDynamic(this, &ASAICharacter::OnPawnInSight);
	AttributesComponent->OnHealthAttributeChanged.AddDynamic(this, &ASAICharacter::OnHealthChanged);
}

void ASAICharacter::OnPawnInSight(APawn* InPawn)
{
	if (ASAIController* AIController = GetController<ASAIController>())
	{
		AIController->SetCurrentTargetActor(InPawn);
		if (bDebug)
		{
			DrawDebugString(GetWorld(), GetActorLocation(), FString::Printf(TEXT("PLAYER SPOTTED: %s"), *InPawn->GetName()), nullptr, FColor::Cyan, 4.0f, true);
		}
	}
}

void ASAICharacter::OnHealthChanged(AActor* AttackerInstigatorActor, class USAttributesComponent* AttributeComponent, float NewHealth, float Delta, const FHitResult& Hit)
{
	if (Delta < 0.0f)
	{
		if (NewHealth <= 0.0f) //died
		{
			if (const ASAIController* AIController = GetController<ASAIController>())
			{
				AIController->GetBrainComponent()->StopLogic("Killed");
			}

			USkeletalMeshComponent* SkeletalMeshComponent = GetMesh();
			SkeletalMeshComponent->SetCollisionProfileName("Ragdoll");
			SkeletalMeshComponent->SetAllBodiesSimulatePhysics(true);
			SetCanBeDamaged(false);
			SetLifeSpan(DestroyAfterKillSeconds);
		}
		else
		{
			if (AttackerInstigatorActor != this)
			{
				GetController<ASAIController>()->SetCurrentTargetActor(AttackerInstigatorActor);
			}
		}
	}
}
