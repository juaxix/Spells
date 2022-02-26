// Spells - xixgames - juaxix - 2021/2022

#include "Gameplay/Actions/SAction.h"

// Photon includes
#include "PhotonJSON.h"

// Spells includes
#include "Gameplay/SActionsComponent.h"
#include "Online/SPhotonCloudObject.h"

void USAction::StartAction_Implementation(AActor* Instigator)
{
	//ensureAlwaysMsgf(!bIsActive, TEXT("An action must be inactive to be able to start it"));
	
	if (OwningActionsComponent)
	{
		OwningActionsComponent->ActiveGameplayTags.AppendTags(GrantTags);
		OwningActionsComponent->OnActionStarted.Broadcast(OwningActionsComponent, this);
	}

	bIsActive = true;

	ReplicateActionState(Instigator);
}

void USAction::StopAction_Implementation(AActor* Instigator)
{
	//ensureAlwaysMsgf(bIsActive, TEXT("An action must be active to be able to stop it"));

	if (USActionsComponent* ActionsComponent = GetOwningComponent())
	{
		ActionsComponent->ActiveGameplayTags.RemoveTags(GrantTags);
		ActionsComponent->OnActionStopped.Broadcast(ActionsComponent, this);
	}

	bIsActive = false;

	ReplicateActionState(Instigator);
}

void USAction::SetActionsOwner(USActionsComponent* InActionsComponent, int32 InActionNetId)
{
	OwningActionsComponent = InActionsComponent;
	ActionNetId = InActionNetId;
}

UPhotonJSON* USAction::GetActionStateJSON(AActor* Instigator)
{
	// We need to get the instigator type and id
	int32 InstigatorUniqueId = -1;
	ESInstigatorTypes InstigatorType = ESInstigatorTypes::INVALID;
	
	OwningActionsComponent->PhotonCloudObject->GetInstigatorUniqueId(Instigator, InstigatorType, InstigatorUniqueId);
	if (InstigatorUniqueId == -1)
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not find a valid unique id for instigator"));

		return nullptr;
	}
	
	return UPhotonJSON::Create(this)->Set_JSON_Object(
			FString::FromInt(ActionNetId), UPhotonJSON::Create(this)
				->SetBoolean(SpellsKeysForReplication::ActionActive, bIsActive)
				->SetInteger(SpellsKeysForReplication::InstigatorUniqueId, InstigatorUniqueId)
				->SetShort(SpellsKeysForReplication::InstigatorType, static_cast<short>(InstigatorType)));
}

void USAction::ReplicateActionState(AActor* Instigator)
{
	if (OwningActionsComponent && OwningActionsComponent->HasPhotonAuthority())
	{
		OwningActionsComponent->ReplicateActionEvent(GetActionStateJSON(Instigator));
	}
}

UPhotonJSON* USAction::ToPhotonJSON_Implementation() const
{
	// we only need the class name to build the action in the replicated side
	return UPhotonJSON::Create()->SetString(
		SpellsKeysForReplication::ActionClassName, 
		FStringClassReference(GetClass()).ToString());
}

void USAction::OnPropertiesChanged_Implementation(const UPhotonJSON* InActionJSON)
{
	if (!InActionJSON)
	{
		return;
	}

	if (InActionJSON->Contains(SpellsKeysForReplication::ActionActive))
	{
		const bool bShouldActive = InActionJSON->GetBoolean(SpellsKeysForReplication::ActionActive);
		if (bShouldActive != bIsActive)
		{
			// Get instigator from replication data
			AActor* Instigator = GetOwningComponent()->PhotonCloudObject->FindInstigatorWithUniqueId(
				static_cast<ESInstigatorTypes>(InActionJSON->GetShort(SpellsKeysForReplication::InstigatorType)),
				InActionJSON->GetInteger(SpellsKeysForReplication::InstigatorUniqueId)
			);

			if (!bIsActive)
			{
				StartAction(Instigator);
			}
			else
			{
				StopAction(Instigator);
			}
		}
	}
}
