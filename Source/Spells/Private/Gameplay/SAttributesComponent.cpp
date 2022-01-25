// Spells - xixgames - juaxix - 2021/2022
#include "Gameplay/SAttributesComponent.h"

// Spells includes
#include "Gameplay/SpellsGameModeBase.h"
//#include "Player/SPlayerState.h"

// cheats : not available for shipping builds
static TAutoConsoleVariable<float> Spells_CVarGlobalDamageMultipler(TEXT("spells.GlobalDamageMultipler"), true, TEXT("Value to Multiply all damages in the game"), ECVF_Cheat);

USAttributesComponent::USAttributesComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
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
	const float NewHealth = FMath::Clamp(Health + Delta, 0.0f, MaxHealth);

	// Only server can change health actually
	if (GetOwner()->HasAuthority())
	{
		Health = NewHealth;
	}

	const float ActualDelta = NewHealth - OldHealth;

	if (ActualDelta != 0.0f && GetOwner()->HasAuthority())
	{
		if (OnHealthAttributeChanged.IsBound())
		{
			OnHealthAttributeChanged.Broadcast(InstigatorActor, this, NewHealth, ActualDelta, Hit);
		}

		if (ActualDelta < 0.0f && NewHealth == 0.0f)
		{
			if (ASpellsGameModeBase* GameMode = GetWorld()->GetAuthGameMode<ASpellsGameModeBase>())
			{
				GameMode->OnActorKilled(GetOwner(), InstigatorActor);
			}
		}
	}
	
	return ActualDelta != 0.0f;
}

bool USAttributesComponent::ApplyManaChange(float Delta, AActor* InstigatorActor, const FHitResult& Hit)
{
	if (!GetOwner()->CanBeDamaged())
	{
		return false;
	}

	if (!GetOwner()->HasAuthority())
	{
		return true;
	}

	const float OldMana = Mana;

	Mana = FMath::Clamp(Mana + Delta, 0.0f, MaxMana);
	const float ActualDelta = Mana - OldMana;

	if (ActualDelta != 0.0f)
	{
		if (OnManaAttributeChanged.IsBound())
		{
			OnManaAttributeChanged.Broadcast(InstigatorActor, this, Mana, ActualDelta, Hit);
		}

		return true;
	}

	return false;
}

bool USAttributesComponent::ApplyRageChange(float Delta, AActor* InstigatorActor, const FHitResult& Hit)
{
	if (!GetOwner()->CanBeDamaged())
	{
		return false;
	}

	if (!GetOwner()->HasAuthority())
	{
		return true;
	}

	const float OldRage = Rage;

	Rage = FMath::Clamp(Rage + Delta, 0.0f, MaxRage);
	const float ActualDelta = Rage - OldRage;

	if (ActualDelta != 0.0f)
	{
		if (OnRageAttributeChanged.IsBound())
		{
			OnRageAttributeChanged.Broadcast(InstigatorActor, this, Rage, ActualDelta, Hit);
		}

		return true;
	}

	return false;
}
