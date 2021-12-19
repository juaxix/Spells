// Spells - xixgames - juaxix - 2021/2022

#include "AI/SAICharacter.h"

/// Unreal includes
#include "BehaviorTree/BlackboardComponent.h"
#include "DrawDebugHelpers.h"
#include "Perception/PawnSensingComponent.h"

/// Spells game includes
#include "AI/SAIController.h"
#include "Player/SAttributesComponent.h"

namespace
{
	const FName SMUZZLE_NAME = TEXT("Muzzle_01");
}

ASAICharacter::ASAICharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	PawnSensingComponent = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComponent"));
	AttributesComponent = CreateDefaultSubobject<USAttributesComponent>(TEXT("AttributesComponent"));
}

void ASAICharacter::BeginPlay()
{
	Super::BeginPlay();

	// cache muzzle
	MuzzleSocket = GetMesh()->GetSocketByName(SMUZZLE_NAME);

	// bind events
	PawnSensingComponent->OnSeePawn.AddDynamic(this, &ASAICharacter::OnPawnInSight);
}

void ASAICharacter::OnPawnInSight(APawn* InPawn)
{
	if (ASAIController* AIController = Cast<ASAIController>(GetController()))
	{
		AIController->SetCurrentTargetActor(InPawn);
		DrawDebugString(GetWorld(), GetActorLocation(), FString::Printf(TEXT("PLAYER SPOTTED: %s"), *InPawn->GetName()), nullptr, FColor::Cyan, 4.0f, true);
	}
}
