// Spells - xixgames - juaxix - 2021/2022

#include "Gameplay/SpellsGameModeBase.h"

// Unreal includes
#include "EnvironmentQuery/EnvQuery.h"
#include "EnvironmentQuery/EnvQueryManager.h"

// Spells includes
#include "AI/SAICharacter.h"
#include "Gameplay/SAttributesComponent.h"
#include "Pickables/SPickableBase.h"
#include "Player/SCharacter.h"
#include "Player/SPlayerState.h"

// Cheats : not available in final builds (shipping)
static TAutoConsoleVariable<bool> Spells_CVarSpawnBots(TEXT("spells.SpawnBots"), true, TEXT("Enable spawning of bots via timer"), ECVF_Cheat);

ASpellsGameModeBase::ASpellsGameModeBase()
{
	PlayerStateClass = ASPlayerState::StaticClass();
}

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

	if (SpawnPickableQuery && SpawnPickableClasses.Num() > 0)
	{
		if (UEnvQueryInstanceBlueprintWrapper* PickableSpotQueryInstance = UEnvQueryManager::RunEQSQuery(
			this, SpawnPickableQuery, this, EEnvQueryRunMode::AllMatching, nullptr))
		{
			PickableSpotQueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &ASpellsGameModeBase::OnPickableSpawnQueryCompleted);
		}
		
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

void ASpellsGameModeBase::OnPickableSpawnQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
	if (QueryStatus != EEnvQueryStatus::Success)
	{
		UE_LOG(LogTemp, Log, TEXT("Spawn pickables point Find failed"));
		return;
	}

	TArray<FVector> PossibleLocations = QueryInstance->GetResultsAsLocations();
	const float MinDistanceSquared = SpawnPickableMinDistance * SpawnPickableMinDistance;
	UWorld* World = GetWorld();

	// Break out if we reached the desired count or if we have no more potential positions remaining to check
	while (SpawnedPickables.Num() < SpawnPickableLimit && PossibleLocations.Num() > 0)
	{
		const int32 RandomLocationIndex = FMath::RandRange(0, PossibleLocations.Num() - 1);
		FVector RandomLocation = PossibleLocations[RandomLocationIndex];
		PossibleLocations.RemoveAt(RandomLocationIndex);

		bool bIsValidLocation = true;
		for (int32 i = 0; i < SpawnedPickables.Num() && bIsValidLocation; i++)
		{
			if (FVector::DistSquared(SpawnedPickables[i]->GetActorLocation(), RandomLocation) < MinDistanceSquared)
			{
				bIsValidLocation = false;
			}
		}

		if (!bIsValidLocation)
		{
			continue;
		}

		SpawnedPickables.Add(World->SpawnActor<ASPickableBase>(
				SpawnPickableClasses[FMath::RandRange(0, SpawnPickableClasses.Num() - 1)],
				RandomLocation,
				FRotator::ZeroRotator));
	}
}

void ASpellsGameModeBase::KillAllBots()
{
#if !UE_BUILD_SHIPPING
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	for (const ASAICharacter* Bot: SpawnedBots)
	{
		Bot->GetAttributesComponent()->Kill(PlayerController);
	}
#endif
}

void ASpellsGameModeBase::OnActorKilled(AActor* Killed, AActor* Killer)
{
	if (const ASCharacter* KilledPlayer = Cast<ASCharacter>(Killed))
	{
		FTimerHandle SpawnPlayerTimerHandle;
		FTimerDelegate SpawnPlayerTimerDelegate;
		APlayerController* PlayerController = KilledPlayer->GetController<APlayerController>();
		check(PlayerController);
		SpawnPlayerTimerDelegate.BindUObject(this, &AGameModeBase::RestartPlayer, static_cast<AController*>(PlayerController));
		PlayerController->UnPossess();
		Killed->SetLifeSpan(RespawnPlayerTime + 0.6f);
		GetWorldTimerManager().SetTimer(SpawnPlayerTimerHandle, SpawnPlayerTimerDelegate, RespawnPlayerTime, false);
	}

	if (const APawn* KillerPawn = Cast<APawn>(Killer))
	{
		if (ASPlayerState* PlayerState = KillerPawn->GetPlayerState<ASPlayerState>())
		{
			PlayerState->GrantCredits(CreditsGrantedPerKill);
		}
	}
}

void ASpellsGameModeBase::SpawnBotTimerElapsed()
{
	if (!Spells_CVarSpawnBots.GetValueOnAnyThread())
	{
		return;
	}

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
