// Spells - xixgames - juaxix - 2021/2022

#include "Gameplay/Actions/SAction.h"

// Spells includes
#include "Gameplay/SActionsComponent.h"

void USAction::StartAction_Implementation(AActor* Instigator)
{
	//ensureAlwaysMsgf(!bIsActive, TEXT("An action must be inactive to be able to start it"));
	
	if (USActionsComponent* ActionsComponent = GetOwningComponent())
	{
		ActionsComponent->ActiveGameplayTags.AppendTags(GrantTags);
		ActionsComponent->OnActionStarted.Broadcast(ActionsComponent, this);
	}

	ActionRepData.bIsRunning = true;
}

void USAction::StopAction_Implementation(AActor* Instigator)
{
	//ensureAlwaysMsgf(bIsActive, TEXT("An action must be active to be able to stop it"));

	if (USActionsComponent* ActionsComponent = GetOwningComponent())
	{
		ActionsComponent->ActiveGameplayTags.RemoveTags(GrantTags);
		ActionsComponent->OnActionStopped.Broadcast(ActionsComponent, this);
	}

	ActionRepData.bIsRunning = false;
}
