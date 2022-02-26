// Spells - xixgames - juaxix - 2021/2022

#include "AI/SAIController.h"

// Unreal includes
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Kismet/GameplayStatics.h"

// Photon includes
#include "PhotonCloudSubsystem.h"

// Spells includes

#include "Online/SPhotonCloudObject.h"

void ASAIController::BeginPlay()
{
	Super::BeginPlay();
	
	if (PhotonCloudObject)
	{
		SetupBehavior();
		PhotonCloudObject->OnMasterChanged.AddDynamic(this, &ASAIController::OnPhotonMasterPlayerChanged);
	}
}

void ASAIController::SetupBehavior()
{
	// cache blackboard key+vars
	UBehaviorTree* DefaultBehaviorTree = PhotonCloudObject && PhotonCloudObject->AmIMaster() ? MasterBehaviorTree : ClientBehaviorTree;
	if (ensureMsgf(DefaultBehaviorTree, TEXT("Behavior Tree is not set; Please assign it for this AI Controller %s"), *GetName()))
	{
		RunBehaviorTree(DefaultBehaviorTree);
		if (const UBlackboardComponent* BlackboardComponent = GetBlackboardComponent())
		{
			MoveToActorBBKeyID = BlackboardComponent->GetKeyID(SpellsAIController::STARGET_ACTOR_BB_KEY);
			MoveToLocationBBKeyID = BlackboardComponent->GetKeyID(SpellsAIController::STARGET_LOCATION_BB_KEY);
		}
	}
}

void ASAIController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (const UWorld* World = GetWorld())
	{
#if WITH_EDITOR
		if (World->WorldType == EWorldType::Editor || World->WorldType == EWorldType::EditorPreview)
		{
			return;
		}
#endif
		// cache our Spells version of the photon cloud object pointer
		if (const UPhotonCloudSubsystem* PhotonSubsystem = World->GetGameInstance()->GetSubsystem<UPhotonCloudSubsystem>())
		{
			PhotonCloudObject = Cast<USPhotonCloudObject>(PhotonSubsystem->GetPhotonCloudAPI());
		}
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
