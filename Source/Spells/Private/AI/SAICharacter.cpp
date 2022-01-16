// Spells - xixgames - juaxix - 2021/2022

#include "AI/SAICharacter.h"

// Unreal includes
#include "Components/CapsuleComponent.h"
#include "BrainComponent.h"
#include "DrawDebugHelpers.h"
#include "Perception/PawnSensingComponent.h"

// Spells game includes
#include "AI/SAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Gameplay/SActionsComponent.h"
#include "Gameplay/SAttributesComponent.h"
#include "UI/SWorldUserWidget.h"

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
	ActionsComponent = CreateDefaultSubobject<USActionsComponent>("ActionsComponent");
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);
}

void ASAICharacter::BeginPlay()
{
	Super::BeginPlay();

	ApplyAggroLevelChange(nullptr, ESAIAggroLevels::IDLE);

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
		if (AIController->GetCurrentTargetActor() == InPawn)
		{
			return;
		}

		AIController->SetCurrentTargetActor(InPawn);
		if (!SpottedWidget)
		{
			SpottedWidget = CreateWidget<USWorldUserWidget>(GetWorld(), SpottedWidgetClass);
			if (SpottedWidget)
			{
				SpottedWidget->AttachedActor = this;
				SpottedWidget->AddToViewport(10); // if ZOrder > 0 place this widget over others to avoid be behind health
			}
		}

		ApplyAggroLevelChange(InPawn, ESAIAggroLevels::SPOTTED);

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

			if (IsValid(SpottedWidget))
			{
				SpottedWidget->RemoveFromParent();
			}

			USkeletalMeshComponent* SkeletalMeshComponent = GetMesh();
			SkeletalMeshComponent->SetCollisionProfileName("Ragdoll");
			SkeletalMeshComponent->SetAllBodiesSimulatePhysics(true);
			GetCharacterMovement()->DisableMovement();
			GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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
