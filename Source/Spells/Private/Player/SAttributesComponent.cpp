// Spells - xixgames - juaxix - 2021
#include "Player/SAttributesComponent.h"

USAttributesComponent::USAttributesComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	
}

bool USAttributesComponent::ApplyHealthChange(float Delta, AActor* InstigatorActor, const FHitResult& Hit)
{
	Health += Delta;
	if (OnHealthAttributeChanged.IsBound())
	{
		OnHealthAttributeChanged.Broadcast(InstigatorActor, this, Health, Delta, Hit);
	}

	return true;
}

