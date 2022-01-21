// Spells - xixgames - juaxix - 2021/2022

#include "Gameplay/SpellsGameModeBase.h"

// Unreal includes
#include "Engine/AssetManager.h"
#include "EngineUtils.h"
#include "EnvironmentQuery/EnvQuery.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"

// Spells includes
#include "AI/SAICharacter.h"
#include "Data/SEnemyDataAsset.h"
#include "Engine/AssetManagerSettings.h"
#include "Gameplay/SAttributesComponent.h"
#include "Gameplay/SInteractableInterface.h"
#include "Gameplay/SSaveGame.h"
#include "Pickables/SPickableBase.h"
#include "Player/SCharacter.h"
#include "Player/SPlayerState.h"


// Cheats : not available in final builds (shipping)
static TAutoConsoleVariable<bool> Spells_CVarSpawnEnemies(TEXT("spells.SpawnEnemies"), true, TEXT("Enable spawning of enemies via timer"), ECVF_Cheat);

ASpellsGameModeBase::ASpellsGameModeBase()
{
	PlayerStateClass = ASPlayerState::StaticClass();
}

void ASpellsGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	LoadSaveGame();
}

void ASpellsGameModeBase::StartPlay()
{
	Super::StartPlay();

	if (TimerHandle_SpawnEnemies.IsValid())
	{
		TimerHandle_SpawnEnemies.Invalidate();
	}

	if (IsValid(SpawnEnemyQuery))
	{
		GetWorldTimerManager().SetTimer(TimerHandle_SpawnEnemies, this, &ASpellsGameModeBase::SpawnEnemyTimerElapsed, SpawnEnemyTimerInterval, true);
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

void ASpellsGameModeBase::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);

	if (ASPlayerState* PS = NewPlayer->GetPlayerState<ASPlayerState>())
	{
		PS->LoadPlayerState(CurrentSaveGame);
	}
}

bool ASpellsGameModeBase::CanSpawnEnemy() const
{
	if (!IsValid(SpawnEnemyInTimeCurve))
	{
		return false;
	}
	const float MaxNumberOfEnemies = SpawnEnemyInTimeCurve->GetFloatValue(GetWorld()->TimeSeconds);

	if (SpawnedEnemies.Num() < MaxNumberOfEnemies)
	{
		return true;
	}

	int32 NumAlive = 0;
	for (const ASAICharacter* Enemy: SpawnedEnemies)
	{
		if (IsValid(Enemy) && Enemy->GetAttributesComponent()->IsAlive())
		{
			NumAlive++;
		}
	}

	return NumAlive < MaxNumberOfEnemies;
}

void ASpellsGameModeBase::OnSpawnEnemyEQ_Completed(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
	if (QueryStatus != EEnvQueryStatus::Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawn Enemy EQS Query Failed!"));
	}
	else
	{
		if (!CanSpawnEnemy())
		{
			return;
		}

		const TArray<FVector>& Locations = QueryInstance->GetResultsAsLocations();
		if (Locations.Num() > 0)
		{
			if (IsValid(EnemiesDataTable) && EnemiesDataTable->GetRowMap().Num() > 0)
			{
				TArray<FSEnemyInfoRow*> EnemyInfoRows;
				EnemiesDataTable->GetAllRows("", EnemyInfoRows);
				// Get a random enemy from the table
				const int32 RandomIndex = FMath::RandRange(0, EnemyInfoRows.Num() - 1);
				FSEnemyInfoRow* RandomEnemyInfoRow = EnemyInfoRows.IsValidIndex(RandomIndex)
											? EnemyInfoRows[RandomIndex]
											: nullptr;

				if (UAssetManager* AssetManager = UAssetManager::GetIfValid())
				{
					const FStreamableDelegate AssetsLoadedCallback = FStreamableDelegate::CreateUObject(this, 
						&ASpellsGameModeBase::OnAssetsLoaded, RandomEnemyInfoRow->EnemyAssetId, Locations[0]);
					AssetManager->LoadPrimaryAsset(RandomEnemyInfoRow->EnemyAssetId, TArray<FName>(), AssetsLoadedCallback);
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("No enemies defined in the DataTable of Enemies to spawn"));
			}
		}
	}
}

void ASpellsGameModeBase::OnAssetsLoaded(FPrimaryAssetId LoadedAssetId, FVector SpawnLocation)
{
	USEnemyDataAsset* EnemyData = nullptr;
	if (const UAssetManager* AssetManager = UAssetManager::GetIfValid())
	{
		EnemyData = Cast<USEnemyDataAsset>(AssetManager->GetPrimaryAssetObject(LoadedAssetId));
	}

	if (EnemyData && EnemyData->EnemyClass)
	{
		FActorSpawnParameters ActorSpawnParameters;
		ActorSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		const int32 NewEnemyIndex = SpawnedEnemies.Emplace(GetWorld()->SpawnActor<ASAICharacter>(
			EnemyData->EnemyClass, 
			SpawnLocation, 
			FRotator::ZeroRotator, 
			ActorSpawnParameters));

		if (NewEnemyIndex > -1)
		{
			ASAICharacter* EnemyCharacter = SpawnedEnemies[NewEnemyIndex];
			if (NewEnemyIndex == 0) // only log for the first one
			{
				UE_LOG(LogTemp, Log, TEXT("Spawned enemy: %s with enemy data: %s"), 
					*GetNameSafe(EnemyCharacter), 
					*GetNameSafe(EnemyData));
			}

			// grant actions to the spawned enemy
			if (USActionsComponent* EnemyActionsComp = EnemyCharacter->GetActionsComponent())
			{
				for (const TSubclassOf<USAction>& ActionClass : EnemyData->Actions)
				{
					EnemyActionsComp->AddAction(EnemyCharacter, ActionClass);
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Could not spawn Enemy for AssetId %s"), *LoadedAssetId.ToString());
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No enemy class defined in the DataTable of Enemies to spawn for AssetId %s"), *LoadedAssetId.ToString());
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

void ASpellsGameModeBase::KillAllEnemies()
{
#if !UE_BUILD_SHIPPING
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	for (const ASAICharacter* Enemy: SpawnedEnemies)
	{
		Enemy->GetAttributesComponent()->Kill(PlayerController);
	}

	SpawnedEnemies.Empty();
#endif
}

void ASpellsGameModeBase::OnActorKilled(AActor* Killed, AActor* Killer)
{
	if (const ASCharacter* KilledCharacter = Cast<ASCharacter>(Killed))
	{
		FTimerHandle SpawnPlayerTimerHandle;
		FTimerDelegate SpawnPlayerTimerDelegate;
		APlayerController* PlayerController = KilledCharacter->GetController<APlayerController>();
		check(PlayerController);
		SpawnPlayerTimerDelegate.BindUObject(this, &AGameModeBase::RestartPlayer, static_cast<AController*>(PlayerController));
		PlayerController->UnPossess();
		Killed->SetLifeSpan(RespawnPlayerTime + 0.6f);
		GetWorldTimerManager().SetTimer(SpawnPlayerTimerHandle, SpawnPlayerTimerDelegate, RespawnPlayerTime, false);
	}
	else if (ASAICharacter* KilledAICharacter = Cast<ASAICharacter>(Killed))
	{
		SpawnedEnemies.Remove(KilledAICharacter);
	}

	if (const APawn* KillerPawn = Cast<APawn>(Killer))
	{
		if (ASPlayerState* PlayerState = KillerPawn->GetPlayerState<ASPlayerState>())
		{
			PlayerState->GrantCredits(CreditsGrantedPerKill);
		}
	}
}

void ASpellsGameModeBase::WriteSaveGame()
{
	// we use player states instead of an unique user ID in the case of Steam, EOS, PSN, GooglePlay and so on
	for (APlayerState* PlayerState : GameState->PlayerArray)
	{
		if (ASPlayerState* SPS = Cast<ASPlayerState>(PlayerState))
		{
			SPS->SavePlayerState(CurrentSaveGame);
			break; // TODO : add multi-player support
		}
	}

	CurrentSaveGame->SavedActors.Empty(CurrentSaveGame->SavedActors.Num());

	// add our interactable actors to the savegame
	for (FActorIterator ItActor(GetWorld()); ItActor; ++ItActor)
	{
		AActor* Actor = *ItActor;
		if (IsValid(Actor) && Actor->Implements<USInteractableInterface>())
		{
			FSActorSaveData ActorData{Actor->GetName(), Actor->GetActorTransform()};
			FMemoryWriter MemoryWriter(ActorData.ByteData);
			FObjectAndNameAsStringProxyArchive Ar(MemoryWriter, true);
			Ar.ArIsSaveGame = true; // Find only vars with the flag SaveGame (UPROPERTY)
			Actor->Serialize(Ar);
			CurrentSaveGame->SavedActors.Emplace(ActorData);
		}
	}

	UGameplayStatics::SaveGameToSlot(CurrentSaveGame, SaveGameSlotName, 0);
}

void ASpellsGameModeBase::LoadSaveGame()
{
	if (UGameplayStatics::DoesSaveGameExist(SaveGameSlotName, 0))
	{
		if (USSaveGame* SaveGame = Cast<USSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveGameSlotName, 0)))
		{
			CurrentSaveGame = SaveGame;
			for (FActorIterator ItActor(GetWorld()); ItActor; ++ItActor)
			{
				AActor* Actor = *ItActor;
				if (IsValid(Actor) && Actor->Implements<USInteractableInterface>())
				{
					CurrentSaveGame->SavedActors.Emplace(FSActorSaveData{ Actor->GetName(), Actor->GetActorTransform() });
					if (const FSActorSaveData* SavedActorIt = CurrentSaveGame->SavedActors.FindByPredicate(
						[Actor](const FSActorSaveData& A)->bool{return A.ActorName.Equals(Actor->GetName());}))
					{
						Actor->SetActorTransform(SavedActorIt->Transform);
						if (SavedActorIt->ByteData.Num() > 0)
						{
							FMemoryReader MemoryReader(SavedActorIt->ByteData);
							FObjectAndNameAsStringProxyArchive Ar(MemoryReader, true);
							Ar.ArIsSaveGame = true; // Find only vars with the flag SaveGame (UPROPERTY)
							Actor->Serialize(Ar);
							ISInteractableInterface::Execute_OnRestoredStateFromSaveGame(Actor);
						}
					}
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to load SaveGame %s"), *SaveGameSlotName);
		}
	}
	else
	{
		CurrentSaveGame = Cast<USSaveGame>(UGameplayStatics::CreateSaveGameObject(USSaveGame::StaticClass()));
	}
}

void ASpellsGameModeBase::SpawnEnemyTimerElapsed()
{
	if (!Spells_CVarSpawnEnemies.GetValueOnAnyThread())
	{
		return;
	}

	if (ensureMsgf(SpawnEnemyQuery, TEXT("Set the Spawn Enemy EQS to be able to spawn enemies in the Game Mode")))
	{
		if (!CanSpawnEnemy())
		{
			return;
		}

		UEnvQueryInstanceBlueprintWrapper* EQ_BP_Wrapper = UEnvQueryManager::RunEQSQuery(this, SpawnEnemyQuery, this, EEnvQueryRunMode::RandomBest5Pct, nullptr /** result in blueprint not needed */);
		if (ensure(EQ_BP_Wrapper))
		{
			// wait for results - bind
			EQ_BP_Wrapper->GetOnQueryFinishedEvent().AddDynamic(this,&ASpellsGameModeBase::OnSpawnEnemyEQ_Completed);
		}
	}
}
