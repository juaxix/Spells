// Spells - xixgames - juaxix - 2021/2022
#include "Gameplay/SAttributesComponent.h"

USAttributesComponent::USAttributesComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	
}

bool USAttributesComponent::ApplyHealthChange(float Delta, AActor* InstigatorActor, const FHitResult& Hit)
{
	const float OldHealth = Health;

	Health = FMath::Clamp(Health + Delta, 0.0f, MaxHealth);

	if (OnHealthAttributeChanged.IsBound())
	{
		OnHealthAttributeChanged.Broadcast(InstigatorActor, this, Health, Health - OldHealth, Hit);
	}

	return true;
}

