// Spells - xixgames - juaxix - 2021/2022
#include "Gameplay/SAttributesComponent.h"

// Spells includes
#include "Gameplay/SpellsGameModeBase.h"
#include "Player/SPlayerState.h"

// cheats : not available for shipping builds
static TAutoConsoleVariable<float> Spells_CVarGlobalDamageMultipler(TEXT("spells.GlobalDamageMultipler"), true, TEXT("Value to Multiply all damages in the game"), ECVF_Cheat);

USAttributesComponent::USAttributesComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool USAttributesComponent::ApplyHealthChange(float Delta, AActor* InstigatorActor, const FHitResult& Hit)
{
	if (!GetOwner()->CanBeDamaged())
	{
		return false;
	}

	if (Delta < 0.0f)
	{
		Delta *= Spells_CVarGlobalDamageMultipler.GetValueOnAnyThread();
	}

	const float OldHealth = Health;

	Health = FMath::Clamp(Health + Delta, 0.0f, MaxHealth);
	const float ActualDelta = Health - OldHealth;

	if (ActualDelta != 0.0f)
	{
		if (OnHealthAttributeChanged.IsBound())
		{
			OnHealthAttributeChanged.Broadcast(InstigatorActor, this, Health, ActualDelta, Hit);
		}

		if (ActualDelta < 0.0f && Health == 0.0f)
		{
			if (ASpellsGameModeBase* GameMode = GetWorld()->GetAuthGameMode<ASpellsGameModeBase>())
			{
				GameMode->OnActorKilled(GetOwner(), InstigatorActor);
			}
		}
	}
	
	return ActualDelta != 0.0f;
}

