// Spells - xixgames - juaxix - 2021/2022

#include "Gameplay/SActionsComponent.h"

// Spells game includes
#include "Gameplay/SAction.h"

USActionsComponent::USActionsComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USActionsComponent::AddAction(TSubclassOf<USAction> ActionClass)
{
	if (!ensure(ActionClass))
	{
		return;
	}

	USAction* NewAction = NewObject<USAction>(this, ActionClass);
	if (ensure(NewAction))
	{
		Actions.Add(NewAction);
	}
}

bool USActionsComponent::StartActionByName(AActor* Instigator, const FName& ActionName)
{
	for (USAction* Action : Actions)
	{
		if (Action && ActionName.IsEqual(Action->ActionName))
		{
			Action->StartAction(Instigator);

			return true;
		}
	}

	return false;
}

bool USActionsComponent::StopActionByName(AActor* Instigator, const FName& ActionName)
{
	for (USAction* Action : Actions)
	{
		if (Action && ActionName.IsEqual(Action->ActionName))
		{
			Action->StopAction(Instigator);
			// Actions.Remove(Action); TODO remove?

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

