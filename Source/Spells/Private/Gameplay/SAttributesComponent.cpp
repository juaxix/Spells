// Spells - xixgames - juaxix - 2021/2022
#include "Gameplay/SAttributesComponent.h"

// Photon includes
#include "PhotonCloudSubsystem.h"

// Spells includes
#include "PhotonJSON.h"
#include "AI/SAICharacter.h"
#include "Gameplay/SpellsGameModeBase.h"
#include "Player/SCharacter.h"

// cheats : not available for shipping builds
static TAutoConsoleVariable<float> Spells_CVarGlobalDamageMultipler(TEXT("spells.GlobalDamageMultipler"), true, TEXT("Value to Multiply all damages in the game"), ECVF_Cheat);

USAttributesComponent::USAttributesComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USAttributesComponent::SetupPhoton(int32 ActorUniqueId, ESInstigatorTypes ActorInstigatorType)
{
	if (!PhotonCloudObject)
	{
		PhotonCloudObject = Cast<USPhotonCloudObject>(GetWorld()->GetGameInstance()->GetSubsystem<UPhotonCloudSubsystem>()->GetPhotonCloudAPI());
	}
	
	if (ActorUniqueId == -1 || ActorInstigatorType == ESInstigatorTypes::INVALID)
	{
		PhotonCloudObject->GetInstigatorUniqueId(GetOwner(), OwnerInstigatorType, OwnerUniqueNetId);
	}
	else
	{
		OwnerUniqueNetId = ActorUniqueId;
		OwnerInstigatorType = ActorInstigatorType;
	}

	LastHealth = Health;
	LastMana = Mana;
	LastRage = Rage;
}

void USAttributesComponent::OnAttributesPropertiesChanged_Implementation(UPhotonJSON* AttributesJSON)
{
	if (!IsValid(AttributesJSON))
	{
		return;
	}

	if (AttributesJSON->Contains(SpellsKeysForReplication::HealthAttribute))
	{
		const float OldHealth = Health;
		Health = AttributesJSON->GetFloat(SpellsKeysForReplication::HealthAttribute);
		const float Delta = Health - OldHealth;
		OnHealthAttributeChanged.Broadcast(nullptr, nullptr, Health, Delta, FHitResult());
		if (Delta < 0.0f && Health == 0.0f)
		{
			if (ASpellsGameModeBase* GameMode = GetWorld()->GetAuthGameMode<ASpellsGameModeBase>())
			{
				GameMode->OnActorKilled(GetOwner(), nullptr);
			}
		}
	}

	if (AttributesJSON->Contains(SpellsKeysForReplication::ManaAttribute))
	{
		const float OldMana = Mana;
		Mana = AttributesJSON->GetFloat(SpellsKeysForReplication::ManaAttribute);
		OnManaAttributeChanged.Broadcast(nullptr, nullptr, Mana, Mana - OldMana, FHitResult());
	}

	if (AttributesJSON->Contains(SpellsKeysForReplication::RageAttribute))
	{
		const float OldRage = Rage;
		Rage = AttributesJSON->GetFloat(SpellsKeysForReplication::RageAttribute);
		OnRageAttributeChanged.Broadcast(nullptr, nullptr, Rage, Rage - OldRage, FHitResult());
	}
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
	const bool bHasPhotonAuth = HasPhotonAuthory();

	if (bHasPhotonAuth)
	{
		Health = NewHealth;
		ReplicateAttributes();
	}

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
				GameMode->OnActorKilled(GetOwner(), nullptr);
			}
		}
	}
	
	return bHasPhotonAuth ? ActualDelta != 0.0f : true;
}

void USAttributesComponent::ApplyManaChange(float Delta, AActor* InstigatorActor, const FHitResult& Hit)
{
	if (!GetOwner()->CanBeDamaged())
	{
		return;
	}

	if (!HasPhotonAuthory())
	{
		return;
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
	}

	ReplicateAttributes();
}

void USAttributesComponent::ApplyRageChange(float Delta, AActor* InstigatorActor, const FHitResult& Hit)
{
	if (!GetOwner()->CanBeDamaged())
	{
		return;
	}

	if (!HasPhotonAuthory())
	{
		return;
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
	}

	ReplicateAttributes();
}

UPhotonJSON* USAttributesComponent::GetAttributesForReplication()
{
	UPhotonJSON* AttributesJSON = UPhotonJSON::Create(this);
	if (Health != LastHealth)
	{
		AttributesJSON->SetFloat(SpellsKeysForReplication::HealthAttribute, Health);
		LastHealth = Health;
	}

	if (Mana != LastMana)
	{
		AttributesJSON->SetFloat(SpellsKeysForReplication::ManaAttribute, Mana);
		LastMana = Mana;
	}

	if (Rage != LastRage)
	{
		AttributesJSON->SetFloat(SpellsKeysForReplication::RageAttribute, Rage);
		LastRage = Rage;
	}

	return AttributesJSON;
}

void USAttributesComponent::ReplicateAttributes()
{
	if (!IsValid(GetOwner()))
	{
		return;
	}

	switch(OwnerInstigatorType)
	{
		case ESInstigatorTypes::INVALID:
		{
			UE_LOG(LogTemp, Warning, TEXT("Can't sync the attributes of this component for %s"), *GetNameSafe(GetOwner()));
			return;
		}
		case ESInstigatorTypes::PLAYER:
		{
			if (const ASCharacter* Character = Cast<ASCharacter>(GetOwner()))
			{
				if (Character->PlayerNumber == OwnerUniqueNetId && OwnerUniqueNetId == PhotonCloudObject->GetPlayerNumber())
				{
					const UPhotonJSON* CharAttributesJSON = GetAttributesForReplication();

					if (CharAttributesJSON->GetCount() > 0)
					{
						PhotonCloudObject->SetPlayerCustomProperties(
							UPhotonJSON::Create(this)
								->Set_JSON_Object(SpellsKeysForReplication::Attributes,CharAttributesJSON));
					}
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Could not replicate attributes for a remote player (only local players can do that)"));
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Could not find the player Nr %d in the joined list"), OwnerUniqueNetId);
			}
			break;
		}
		case ESInstigatorTypes::MASTER_AI:
		{
			if (PhotonCloudObject->AmIMaster())
			{
				if (GetOwner()->IsA(ASAICharacter::StaticClass()))
				{
					const UPhotonJSON* AttributesJSON = GetAttributesForReplication();
					if (AttributesJSON->GetCount() > 0)
					{
						UPhotonJSON* P = UPhotonJSON::Create(this)
							->Set_JSON_Object(
								SpellsKeysForReplication::EnemiesAttributesPrefix + FString::FromInt(OwnerUniqueNetId),
								AttributesJSON);
						PhotonCloudObject->AddCustomRoomProperties(
							P
						);
						UE_LOG(LogTemp, Log, TEXT("Sending AI attrs %s"), *P->GetJSONasString());
					}
				}
				else
				{
					UE_LOG(LogTemp, Warning, 
						TEXT("Can't replicate this actor (%s), it is not supported yet"),
						*GetNameSafe(GetOwner()));
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, 
					TEXT("Could not change an attribute for a master controlled actor from a non-master client (%s)"), 
					*GetNameSafe(GetOwner()))
			}
		}
	}
}
