// Spells - xixgames - juaxix - 2021/2022

#include "Gameplay/SActionsComponent.h"

// Unrea includes
#include "GameFramework/Character.h"

// Photon includes
#include "PhotonCloudAPIBPLibrary.h"
#include "PhotonCloudSubsystem.h"
#include "PhotonJSON.h"

// Spells game includes
#include "Spells/Spells.h"
#include "Gameplay/Actions/SAction.h"
#include "Player/SCharacter.h"

DECLARE_CYCLE_STAT(TEXT("Spells_StartActionByName"), STAT_SpellsTartActionByName, STATGROUP_SPELLS);

void USActionsComponent::SetupPhoton(int32 ActorUniqueId, ESInstigatorTypes ActorInstigatorType)
{
	if (!PhotonCloudObject)
	{
		PhotonCloudObject = Cast<USPhotonCloudObject>(GetWorld()->GetGameInstance()->GetSubsystem<UPhotonCloudSubsystem>()->GetPhotonCloudAPI());
	}

	if (ActorUniqueId == -1 || OwnerInstigatorType == ESInstigatorTypes::INVALID)
	{
		PhotonCloudObject->GetInstigatorUniqueId(GetOwner(), OwnerInstigatorType, OwnerUniqueNetId);
	}
	else
	{
		OwnerUniqueNetId = ActorUniqueId;
		OwnerInstigatorType = ActorInstigatorType;
	}
}

void USActionsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (GEngine)
	{
		if (OwnerInstigatorType == ESInstigatorTypes::PLAYER)
		{
			const FString DebugMsg = GetNameSafe(GetOwner()) + " : " + ActiveGameplayTags.ToStringSimple();
			GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Green, DebugMsg);
		}
		else
		{
			int64 HashedName;
			static TArray<int32> NoPlayerSpecific;
			Cast<IPhotonMechanics>(GetOwner())->Execute_GetHashedName(GetOwner(), HashedName);
			// ID : uniqueid, hashedname, registed in photon object, still in game mode, alive
			FString Id = FString::Printf(TEXT("ID: %d, H: %lld, R: %s, GM: %s"), OwnerUniqueNetId, HashedName,
				GetOwner() == PhotonCloudObject->GetSerializedActor(HashedName) ? TEXT("YES"):TEXT("NO"),
				PhotonCloudObject->FindInstigatorWithUniqueId(ESInstigatorTypes::MASTER_AI, OwnerUniqueNetId) == GetOwner() ? TEXT("YES") : TEXT("NO"));
			GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow, 
				Id + GetNameSafe(GetOwner()) + " : " + ActiveGameplayTags.ToStringSimple());
		}
	}
}

void USActionsComponent::ReplicateActionEvent(UPhotonJSON* ActionEventJSON)
{
	if (!ActionEventJSON)
	{
		return;
	}

	if (OwnerInstigatorType == ESInstigatorTypes::PLAYER)
	{
		PhotonCloudObject->SendPlayerData(UPhotonJSON::Create(this)->Set_JSON_Object(
				 SpellsKeysForReplication::ActionEvent, ActionEventJSON), TArray<int32>());
		UE_LOG(LogTemp, VeryVerbose, TEXT("Sending Player Action Event %s"), *ActionEventJSON->GetJSONasString());
	}
	else
	{
		int64 HashedName;
		static TArray<int32> NoPlayerSpecific;
		Cast<IPhotonMechanics>(GetOwner())->Execute_GetHashedName(GetOwner(), HashedName);
		PhotonCloudObject->SendActorSerializedData(HashedName, 
			UPhotonJSON::Create(this)
					->Set_JSON_Object(
						SpellsKeysForReplication::EnemiesActionsEventPrefix + 
						FString::FromInt(OwnerUniqueNetId), ActionEventJSON), 
				NoPlayerSpecific);
		UE_LOG(LogTemp, Log, TEXT("Sending AI Action Event %s"), *ActionEventJSON->GetJSONasString());
	}
}

void USActionsComponent::SyncActions()
{
	if (!HasPhotonAuthority())
	{
		return;
	}

	UPhotonJSON* ActionsJSON = UPhotonJSON::Create(this);
	// create a list consisting in the action net ids with the class name
	// list JSON like: { "actions": { "1": {"ClassName": "SprintAction"}, "2": {"ClassName":"OtherAction" } } }
	for (const USAction* Action : Actions)
	{
		ActionsJSON->Set_JSON_Object(
			FString::FromInt(Action->ActionNetId), // unique Id (created with the actor components container) of the action
			Action->ToPhotonJSON() // adds the minimum info needed to build the action in the other side
		);
	}

	if (OwnerInstigatorType == ESInstigatorTypes::PLAYER)
	{
		PhotonCloudObject->SetPlayerCustomProperties(
			UPhotonJSON::Create(this)
				->Set_JSON_Object(SpellsKeysForReplication::Actions, ActionsJSON));
	}
	else if (OwnerInstigatorType == ESInstigatorTypes::MASTER_AI)
	{
		// A master AI has only an actions component but as we use room properties instead of player properties,
		// we need to use a prefix for when the action list change
		PhotonCloudObject->AddCustomRoomProperties(
			UPhotonJSON::Create(this)
				->Set_JSON_Object(SpellsKeysForReplication::EnemiesActionsPrefix + FString::FromInt(OwnerUniqueNetId),
					UPhotonJSON::Create(this)
						->Set_JSON_Object(SpellsKeysForReplication::Actions, ActionsJSON)));
	}
}

bool USActionsComponent::HasPhotonAuthority() const
{
	if (OwnerInstigatorType == ESInstigatorTypes::PLAYER)
	{
		return  OwnerUniqueNetId == PhotonCloudObject->GetPlayerNumber() || 
				Cast<ASCharacter>(GetOwner())->IsLocallyControlled();
	}

	return PhotonCloudObject->AmIMaster();
}

void USActionsComponent::OnActionsPropertiesChanged_Implementation(UPhotonJSON* ActionsJSON)
{
	if (!IsValid(ActionsJSON))
	{
		return;
	}

	TArray<FString> Keys;
	ActionsJSON->Data->Values.GetKeys(Keys);
	for (const FString& Key : Keys)
	{
		if (ActionsJSON->DataType(Key) != EJson::Object)
		{
			continue;
		}

		const UPhotonJSON* ActionJSON = ActionsJSON->Get_JSON_Object(Key);
		if (!ActionJSON)
		{
			continue;
		}

		const int32 ActionNetId = FCString::Atoi(*Key);
		if (USAction** Action = Actions.FindByPredicate(
		[ActionNetId](const USAction* Action) -> bool { return IsValid(Action) && Action->ActionNetId == ActionNetId; }))
		{
			(*Action)->OnPropertiesChanged(ActionJSON);
		}
		else
		{
			//Just create the action if it is the first replication, the JSON will contain just the action class name to instantiate it here (replicated side)
			if (ActionJSON->Contains(SpellsKeysForReplication::ActionClassName))
			{
				UClass* ActionClass = UPhotonCloudAPIBPLibrary::GetUClassFromString(ActionJSON->GetString(SpellsKeysForReplication::ActionClassName));
				if (ActionClass && ActionClass->IsChildOf(USAction::StaticClass()))
				{
					// when replicating an action for first time it will not automatically auto-start (if it is marked to autostart)
					// the start action will be replicated from the owning client autostarting the action, so this instigator is null
					USAction* NewReplicatedAction = AddAction(nullptr, ActionClass);
					NewReplicatedAction->ActionNetId = ActionNetId;
				}
			}
		}
	}
}

bool USActionsComponent::HasAction(TSubclassOf<USAction> ActionClass) const
{
	return Actions.ContainsByPredicate([ActionClass](USAction* Action) -> bool { return Action && Action->IsA(ActionClass); });
}

USAction* USActionsComponent::GetAction(TSubclassOf<USAction> ActionClass) 
{
	if (!ensure(ActionClass))
	{
		return nullptr;
	}
	
	if (USAction** Action = Actions.FindByPredicate([ActionClass](const USAction* A)->bool{return A && A->IsA(ActionClass);}))
	{
		return *Action;
	}

	return nullptr;
}

USAction* USActionsComponent::AddAction(AActor* Instigator, TSubclassOf<USAction> ActionClass)
{
	if (!ensure(ActionClass))
	{
		return nullptr;
	}

	USAction* NewAction = NewObject<USAction>(GetOwner(), ActionClass, FName(FString::Printf(TEXT("%s_%d"), *ActionClass->GetName(), Actions.Num())));
	if (ensure(NewAction))
	{
		NewAction->SetActionsOwner(this, HasPhotonAuthority() ? ++ActionNetIdCounter : -1);
		Actions.Add(NewAction);

		if (Instigator != nullptr &&
			NewAction->IsAutoStart() && ensureAlwaysMsgf(
			NewAction->CanStart(Instigator), TEXT("Action %s can't be auto-started by Instigator"), *GetNameSafe(NewAction)))
		{
			if (HasPhotonAuthority())
			{
				// replicate the creation of the new action now before starting it so the other clients also has it
				SyncActions();
			}

			NewAction->StartAction(Instigator);
		}
	}

	return NewAction;
}

void USActionsComponent::CreateDefaultActions()
{
	if (Actions.Num() == 0 && PhotonCloudObject && HasPhotonAuthority())
	{
		if (DefaultActions.Num() > 0)
		{
			for (const TSubclassOf<USAction>& SubAction : DefaultActions)
			{
				AddAction(GetOwner(), SubAction);
			}

			SyncActions();
		}
	}
}

void USActionsComponent::RemoveAction(USAction* Action)
{
	ensureAlwaysMsgf(Action && !Action->IsActive(), TEXT("Action must be != nullptr and not active to be removed"));
	Actions.Remove(Action);
	SyncActions();
}

bool USActionsComponent::StartActionByName(AActor* Instigator, const FName& ActionName)
{
	SCOPE_CYCLE_COUNTER(STAT_SpellsTartActionByName);

	for (USAction* Action : Actions)
	{
		if (Action && ActionName.IsEqual(Action->ActionName))
		{
			if (Action->CanStart(Instigator))
			{
				Action->StartAction(Instigator);
				return true;
			}

			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, 
					FString::Printf(TEXT("Failed to execute Action %s"), *ActionName.ToString()));
			}
			
		}
	}

	return false;
}

bool USActionsComponent::StopActionByName(AActor* Instigator, const FName& ActionName)
{
	for (USAction* Action : Actions)
	{
		if (Action && ActionName.IsEqual(Action->ActionName) && Action->IsActive())
		{
			Action->StopAction(Instigator);

			return true;
		}
	}

	return false;
}

void USActionsComponent::ReceiveAnimNotif(AActor* Instigator, const FName& ActionName)
{
	for (USAction* Action : Actions)
	{
		if (Action && ActionName.IsEqual(Action->ActionName))
		{
			Action->ReceiveAnimationNotif();

			return;
		}
	}
}
