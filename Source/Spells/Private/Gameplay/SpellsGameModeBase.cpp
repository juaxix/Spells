// Spells - xixgames - juaxix - 2021/2022

#include "Gameplay/SpellsGameModeBase.h"

// Unreal includes
#include "EnvironmentQuery/EnvQuery.h"
#include "EnvironmentQuery/EnvQueryManager.h"

// Spells includes
#include "AI/SAICharacter.h"
#include "Player/SAttributesComponent.h"

void ASpellsGameModeBase::StartPlay()
{
	Super::StartPlay();

	if (TimerHandle_SpawnBots.IsValid())
	{
		TimerHandle_SpawnBots.Invalidate();
	}

	if (IsValid(SpawnBotQuery))
	{
		GetWorldTimerManager().SetTimer(TimerHandle_SpawnBots, this, &ASpellsGameModeBase::SpawnBotTimerElapsed, SpawnBotTimerInterval, true);
	}
}

bool ASpellsGameModeBase::CanSpawnBot() const
{
	if (!IsValid(SpawnBotInTimeCurve))
	{
		return false;
	}
	const float MaxNumberOfBots = SpawnBotInTimeCurve->GetFloatValue(GetWorld()->TimeSeconds);

	if (SpawnedBots.Num() < MaxNumberOfBots)
	{
		return true;
	}

	int32 NumAlive = 0;
	for (const ASAICharacter* Bot: SpawnedBots)
	{
		if (IsValid(Bot) && Bot->GetAttributesComponent()->IsAlive())
		{
			NumAlive++;
		}
	}

	return NumAlive < MaxNumberOfBots;
}

void ASpellsGameModeBase::OnSpawnBotEQ_Completed(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
	if (QueryStatus != EEnvQueryStatus::Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawn Bot EQS Query Failed!"));
	}
	else
	{
		if (!CanSpawnBot())
		{
			return;
		}

		const TArray<FVector>& Locations = QueryInstance->GetResultsAsLocations();
		if (Locations.Num() > 0)
		{
			FActorSpawnParameters ActorSpawnParameters;
			ActorSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
			SpawnedBots.Add(GetWorld()->SpawnActor<ASAICharacter>(SpawnBotClass, Locations[0], FRotator::ZeroRotator, ActorSpawnParameters));
		}
	}
}

void ASpellsGameModeBase::SpawnBotTimerElapsed()
{
	if (ensureMsgf(SpawnBotQuery, TEXT("Set the Spawn Bot EQS to be able to spawn bots in the Game Mode")))
	{
		if (!CanSpawnBot())
		{
			return;
		}

		UEnvQueryInstanceBlueprintWrapper* EQ_BP_Wrapper = UEnvQueryManager::RunEQSQuery(this, SpawnBotQuery, this, EEnvQueryRunMode::RandomBest5Pct, nullptr /** result in blueprint not needed */);
		if (ensure(EQ_BP_Wrapper))
		{
			// wait for results - bind
			EQ_BP_Wrapper->GetOnQueryFinishedEvent().AddDynamic(this,&ASpellsGameModeBase::OnSpawnBotEQ_Completed);
		}
	}
}
