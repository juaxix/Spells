// Spells - xixgames - juaxix - 2021/2022

#include "Gameplay/SActionsComponent.h"

// Spells game includes
#include "Gameplay/Actions/SAction.h"

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

