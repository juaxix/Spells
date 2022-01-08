// Spells - xixgames - juaxix - 2021/2022

#include "Gameplay/SActionsComponent.h"

// Spells game includes
#include "Gameplay/SAction.h"

#pragma optimize("", off)
void USActionsComponent::BeginPlay()
{
	Super::BeginPlay();
	const UWorld* World = GetWorld();

	if (DefaultActions.Num() > 0 && !GIsCookerLoadingPackage && !GIsEditorLoadingPackage && !HasAnyFlags(RF_ClassDefaultObject) && IsValid(World) && World->IsGameWorld())
	{
		for (const TSubclassOf<USAction>& SubAction : DefaultActions)
		{
			AddAction(SubAction);
		}
	}
}
#pragma optimize("", on)

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

