// Spells - xixgames - juaxix - 2021/2022

#include "Gameplay/Actions/SAction.h"

// Spells includes
#include "Gameplay/SActionsComponent.h"

void USAction::StartAction_Implementation(AActor* Instigator)
{
	ensureAlwaysMsgf(!bIsActive, TEXT("An action must be inactive to be able to start it"));
	GetOwningComponent()->ActiveGameplayTags.AppendTags(GrantTags);
	bIsActive = true;
}

void USAction::StopAction_Implementation(AActor* Instigator)
{
	ensureAlwaysMsgf(bIsActive, TEXT("An action must be active to be able to stop it"));
	GetOwningComponent()->ActiveGameplayTags.RemoveTags(GrantTags);
	bIsActive = false;
}

USActionsComponent* USAction::GetOwningComponent() const
{
	// the instigator could be anything so we need to use the outer that is the component creator of this action
	return Cast<USActionsComponent>(GetOuter());
}
