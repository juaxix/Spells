// Spells - xixgames - juaxix - 2021/2022

#include "Gameplay/SActionsComponent.h"

// Unrea includes
#include "GameFramework/Character.h"

// Spells game includes
#include "Spells/Spells.h"
#include "Gameplay/Actions/SAction.h"

DECLARE_CYCLE_STAT(TEXT("Spells_StartActionByName"), STAT_SpellsTartActionByName, STATGROUP_SPELLS);

void USActionsComponent::BeginPlay()
{
	Super::BeginPlay();
	const UWorld* World = GetWorld();

	if (DefaultActions.Num() > 0 && !GIsCookerLoadingPackage && !GIsEditorLoadingPackage && !HasAnyFlags(RF_ClassDefaultObject) && IsValid(World) && World->IsGameWorld())
	{
		for (const TSubclassOf<USAction>& SubAction : DefaultActions)
		{
			AddAction(GetOwner(), SubAction);
		}
	}
}

void USActionsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (GEngine)
	{
		ACharacter* Char = Cast<ACharacter>(GetOwner());
		if (Char && Char->IsPlayerControlled())
		{
			const FString DebugMsg = GetNameSafe(GetOwner()) + " : " + ActiveGameplayTags.ToStringSimple();
			GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Green, DebugMsg);
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

void USActionsComponent::AddAction(AActor* Instigator, TSubclassOf<USAction> ActionClass)
{
	if (!ensure(ActionClass))
	{
		return;
	}

	USAction* NewAction = NewObject<USAction>(this, ActionClass, FName(FString::Printf(TEXT("%s_%d"), *ActionClass->GetName(), Actions.Num())));
	if (ensure(NewAction))
	{
		Actions.Add(NewAction);
		
		if (NewAction->IsAutoStart() && ensureAlwaysMsgf(
			NewAction->CanStart(Instigator), TEXT("Action %s can't be auto-started by Instigator"), *GetNameSafe(NewAction)))
		{
			NewAction->StartAction(Instigator);
		}
	}
}

void USActionsComponent::RemoveAction(USAction* Action)
{
	ensureAlwaysMsgf(Action && !Action->IsActive(), TEXT("Action must be != nullptr and not active to be removed"));
	Actions.Remove(Action);
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

