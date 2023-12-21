// Spells - xixgames - juaxix - 2021/2022

#include "AI/SAIController.h"

// Unreal includes
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "BehaviorTree/BehaviorTree.h"

// Photon includes
#include "PhotonCloudSubsystem.h"
#include "PhotonJSON.h"

// Spells includes
#include "AI/SAICharacter.h"
#include "Online/SPhotonCloudObject.h"
#include "Player/SCharacter.h"

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

			if (PhotonCloudObject && PhotonCloudObject->AmIMaster() && IsValid(GetCharacter()))
			{
				const ASAICharacter* AICharacter = Cast<ASAICharacter>(GetCharacter());
				if (!AICharacter || !AICharacter->bInitialSync)
				{
					return false;
				}
					
				ESInstigatorTypes InstigatorType;
				int32 InstigatorUniqueId;
				PhotonCloudObject->GetInstigatorUniqueId(InActor, InstigatorType, InstigatorUniqueId);
				UPhotonJSON* NewTargetActorJSON = UPhotonJSON::Create(this)
					->Set_JSON_Object(
						SpellsKeysForReplication::EnemiesTargetActorPrefix + 
						FString::FromInt(AICharacter->AIUniqueId), 
						UPhotonJSON::Create(this)
							->SetInteger(SpellsKeysForReplication::InstigatorUniqueId, InstigatorUniqueId)
							->SetByte(SpellsKeysForReplication::InstigatorType, static_cast<uint8>(InstigatorType)));

				PhotonCloudObject->AddCustomRoomProperties(NewTargetActorJSON);
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
