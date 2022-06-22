// Spells - xixgames - juaxix - 2021/2022

#include "Gameplay/SpellsGameModeBase.h"

// Unreal includes
#include "Dom/JsonObject.h"
#include "Engine/AssetManager.h"
#include "Engine/AssetManagerSettings.h"
#include "EngineUtils.h"
#include "EnvironmentQuery/EnvQuery.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "GameFramework/GameStateBase.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "TimerManager.h"

// Photon includes
#include "PhotonCloudAPIBPLibrary.h"
#include "PhotonCloudSubsystem.h"
#include "PhotonJSON.h"

// Spells includes
#include "AI/SAICharacter.h"
#include "Data/SEnemyDataAsset.h"
#include "Gameplay/SInteractableInterface.h"
#include "Gameplay/SSaveGame.h"
#include "Gameplay/Actions/SAction.h"
#include "Online/SPhotonCloudObject.h"
#include "Pickables/SPickableBase.h"
#include "Player/SCharacter.h"
#include "Player/SPlayerState.h"


// Cheats : not available in final builds (shipping)
static TAutoConsoleVariable<bool> Spells_CVarSpawnEnemies(TEXT("spells.SpawnEnemies"), true, TEXT("Enable spawning of enemies via timer"), ECVF_Cheat);

ASpellsGameModeBase::ASpellsGameModeBase()
{
	PlayerStateClass = ASPlayerState::StaticClass();
}

void ASpellsGameModeBase::RestartPlayer(AController* NewPlayer)
{
	Super::RestartPlayer(NewPlayer);

	if (!PhotonCloudObject || PhotonCloudObject->GetState() != EPhotonCloudStates::JOINED)
	{
		return;
	}

	if (ASCharacter* Character = Cast<ASCharacter>(NewPlayer->GetPawn()))
	{
		const int32 PlayerNumber = PhotonCloudObject->GetPlayerNumber();
		if (!PhotonCloudObject->JoinedCharacters.Contains(PlayerNumber))
		{
			PhotonCloudObject->JoinedCharacters.Add(PlayerNumber, Character);
			Character->SetupPhotonPlayer(PlayerNumber, PhotonCloudObject->GetPlayerName(PlayerNumber), true);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to create a new char"));
	}
	
	PhotonCloudObject->SendPlayerData(UPhotonJSON::Create(this)->SetBoolean(SpellsKeysForReplication::RespawnPlayer, true), TArray<int32>());
}

void ASpellsGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	PhotonCloudObject = Cast<USPhotonCloudObject>(GetGameInstance()->GetSubsystem<UPhotonCloudSubsystem>()->GetPhotonCloudAPI());
	if (PhotonCloudObject->AmIMaster())
	{
		PhotonCloudObject->AddCustomRoomProperties(
			UPhotonJSON::Create(this)
			 ->SetInteger(SpellsKeysForReplication::GameModeStartTime, PhotonCloudObject->GetServerTime()));
	}

	FString SelectedSaveSlot = UGameplayStatics::ParseOption(Options, "SaveGame");
	if (SelectedSaveSlot.Len() > 0)
	{
		SaveGameSlotName = SelectedSaveSlot;
	}

	LoadSaveGame();
}

void ASpellsGameModeBase::StartPlay()
{
	Super::StartPlay();
	if (PhotonCloudObject->AmIMaster())
	{
		OnMasterControlGameMode();
	}
	else
	{
		OnClientRebuildGameMode();
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

bool ASpellsGameModeBase::OnCustomRoomEnemiesPropertyChanged(const FString& EnemyPropertyName, UPhotonJSON* RoomEnemyPropertyJSON)
{
	{// new enemies defined as properties
	static const int32 EnemiesStringLen = FCString::Strlen(SpellsKeysForReplication::EnemiesPrefix);
	if (EnemyPropertyName.Mid(0, EnemiesStringLen).Equals(SpellsKeysForReplication::EnemiesPrefix))
	{
		const int32 EnemyUniqueId = FCString::Atoi(*EnemyPropertyName.Mid(EnemiesStringLen, EnemyPropertyName.Len() - EnemiesStringLen));
		if (!SpawnedEnemies.Contains(EnemyUniqueId))
		{
			ClientSpawnEnemy(EnemyUniqueId, RoomEnemyPropertyJSON->Get_JSON_Object(EnemyPropertyName));
		}

		return true;
	}
	} // enemy props

	{// enemies attributes definitions as properties
	static const int32 EnemiesAttributesStringLen = FCString::Strlen(SpellsKeysForReplication::EnemiesAttributesPrefix);
	if (EnemyPropertyName.Mid(0, EnemiesAttributesStringLen).Equals(SpellsKeysForReplication::EnemiesAttributesPrefix))
	{
		const int32 EnemyUniqueId = FCString::Atoi(*EnemyPropertyName.Mid(EnemiesAttributesStringLen, EnemyPropertyName.Len() - EnemiesAttributesStringLen));
		if (const ASAICharacter* AICharacter = Cast<ASAICharacter>(FindMasterControlledActor(EnemyUniqueId)))
		{
			if (PhotonCloudObject->IsDebugLogOn())
			{
				UE_LOG(LogTemp, Log, TEXT("Getting AI attrs %s"), *RoomEnemyPropertyJSON->Get_JSON_Object(EnemyPropertyName)->GetJSONasString());
			}
			AICharacter->GetAttributesComponent()->OnAttributesPropertiesChanged(RoomEnemyPropertyJSON->Get_JSON_Object(EnemyPropertyName));
		}

		return true;
	}
	} // enemy atts

	{// enemies actions definitions as properties
	static const int32 EnemiesActionsStringLen = FCString::Strlen(SpellsKeysForReplication::EnemiesActionsPrefix);
	if (EnemyPropertyName.Mid(0, EnemiesActionsStringLen).Equals(SpellsKeysForReplication::EnemiesActionsPrefix))
	{
		const int32 EnemyUniqueId = FCString::Atoi(*EnemyPropertyName.Mid(EnemiesActionsStringLen, EnemyPropertyName.Len() - EnemiesActionsStringLen));
		if (const ASAICharacter* AICharacter = Cast<ASAICharacter>(FindMasterControlledActor(EnemyUniqueId)))
		{
			AICharacter->GetActionsComponent()->OnActionsPropertiesChanged(RoomEnemyPropertyJSON->Get_JSON_Object(EnemyPropertyName));
		}

		return true;
	}
	} // enemy actions

	{// enemies changed target
	static const int32 EnemiesTargetActorStringLen = FCString::Strlen(SpellsKeysForReplication::EnemiesTargetActorPrefix);
	if (EnemyPropertyName.Mid(0, EnemiesTargetActorStringLen).Equals(SpellsKeysForReplication::EnemiesTargetActorPrefix))
	{
		const int32 EnemyUniqueId = FCString::Atoi(*EnemyPropertyName.Mid(EnemiesTargetActorStringLen, EnemyPropertyName.Len() - EnemiesTargetActorStringLen));
		if (ASAICharacter* AICharacter = Cast<ASAICharacter>(FindMasterControlledActor(EnemyUniqueId)))
		{
			AICharacter->OnTargetActorChanged(RoomEnemyPropertyJSON->Get_JSON_Object(EnemyPropertyName));
		}

		return true;
	}
	} // enemy target actor

	return false;
}

void ASpellsGameModeBase::OnCustomRoomPropertiesChanged_Implementation(UPhotonJSON* RoomPropertiesJSON)
{
	if (!IsValid(RoomPropertiesJSON))
	{
		return;
	}
	
	TArray<FString> Keys;
	RoomPropertiesJSON->Data->Values.GetKeys(Keys);
	for (const FString& Key : Keys)
	{
		if (Key[0] == 'e')
		{
			if (OnCustomRoomEnemiesPropertyChanged(Key, RoomPropertiesJSON))
			{
				continue;
			}
		}

		// New pickables
		static const int32 PickablesPrefixLen = FCString::Strlen(SpellsKeysForReplication::PickablesPrefix);
		if (Key.Mid(0, PickablesPrefixLen).Equals(SpellsKeysForReplication::PickablesPrefix))
		{
			const int32 PickableUniqueId = FCString::Atoi(*Key.Mid(PickablesPrefixLen, Key.Len() - PickablesPrefixLen));
			if (!SpawnedPickables.Contains(PickableUniqueId))
			{
				ClientSpawnPickable(PickableUniqueId, RoomPropertiesJSON->Get_JSON_Object(Key));
			}

			continue;
		}

		// pickables events
		static const int32 PickableEventPrefixLen = FCString::Strlen(SpellsKeysForReplication::PickableEventPrefix);
		const FString EventPrx = Key.Mid(0, PickableEventPrefixLen);
		if (EventPrx.Equals(SpellsKeysForReplication::PickableEventPrefix))
		{
			const int32 PickableUniqueId = FCString::Atoi(*Key.Mid(PickableEventPrefixLen, Key.Len() - PickableEventPrefixLen));
			if (ASPickableBase* Pickable = FindMasterControlledPickable(PickableUniqueId))
			{
				const UPhotonJSON* PickableEventJSON = RoomPropertiesJSON->Get_JSON_Object(Key);
				if (PickableEventJSON && PickableEventJSON->Contains(SpellsKeysForReplication::PickableIsPicked))
				{
					const int32 PlayerNumber = PickableEventJSON->GetInteger(SpellsKeysForReplication::PickablePicker);
					ASCharacter* Character = PhotonCloudObject->JoinedCharacters.Contains(PlayerNumber)
											? PhotonCloudObject->JoinedCharacters[PlayerNumber]
											: nullptr;
					if (IsValid(Character))
					{
						Pickable->Execute_Pick(Pickable, Character);
					}
				}
			}
		}
	}
}

bool ASpellsGameModeBase::CanSpawnEnemy() const
{
	if (!IsValid(SpawnEnemyInTimeCurve) || !PhotonCloudObject->AmIMaster())
	{
		return false;
	}

	const float MaxNumberOfEnemies = SpawnEnemyInTimeCurve->GetFloatValue(GetWorld()->TimeSeconds);

	if (SpawnedEnemies.Num() < MaxNumberOfEnemies)
	{
		return true;
	}

	int32 NumAlive = 0;
	for (const TTuple<int32, ASAICharacter*>& Enemy : SpawnedEnemies)
	{
		if (IsValid(Enemy.Value) && Enemy.Value->GetAttributesComponent()->IsAlive())
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
				const int32 RandomIndex = FMath::RandRange(0, EnemyInfoRows.Num() - 1);
				const FSEnemyInfoRow* RandomEnemyInfoRow = EnemyInfoRows.IsValidIndex(RandomIndex) ? EnemyInfoRows[RandomIndex] : nullptr;
				if (UAssetManager* AssetManager = UAssetManager::GetIfValid())
				{
					const FStreamableDelegate AssetsLoadedCallback = FStreamableDelegate::CreateUObject(this, &ASpellsGameModeBase::OnEnemyAssetsLoaded,
						RandomEnemyInfoRow->EnemyAssetId, Locations[0], ++CurrentEnemyNetIdCount);
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

void ASpellsGameModeBase::OnEnemyAssetsLoaded(FPrimaryAssetId LoadedAssetId, FVector SpawnLocation, int32 EnemyUniqueId)
{
	USEnemyDataAsset* EnemyData = nullptr;
	if (const UAssetManager* AssetManager = UAssetManager::GetIfValid())
	{
		EnemyData = Cast<USEnemyDataAsset>(AssetManager->GetPrimaryAssetObject(LoadedAssetId));
	}

	if (EnemyData && EnemyData->EnemyClass)
	{
		FTransform SpawnTransform(SpawnLocation);
		SpawnedEnemies.Emplace(
			EnemyUniqueId, 
			GetWorld()->SpawnActorDeferred<ASAICharacter>(
				EnemyData->EnemyClass, SpawnTransform, 
				nullptr, nullptr, 
				ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn)
		);
		
		if (IsValid(SpawnedEnemies[EnemyUniqueId]))
		{
			ASAICharacter* EnemyCharacter = SpawnedEnemies[EnemyUniqueId];
			EnemyCharacter->SetupPhoton(PhotonCloudObject, EnemyUniqueId);
			if (PhotonCloudObject->AmIMaster())
			{
				if (PhotonCloudObject->IsDebugLogOn())
				{
					UE_LOG(LogTemp, Log, TEXT("Master Spawning enemy : ID: %d, Hash: %lld"), EnemyUniqueId, EnemyCharacter->HashedName);
				}

				PhotonCloudObject->AddCustomRoomProperties(UPhotonJSON::Create(this)->Set_JSON_Object(
					SpellsKeysForReplication::EnemiesPrefix + FString::FromInt(EnemyUniqueId),
					UPhotonJSON::Create(this)
					 ->SetString(SpellsKeysForReplication::EnemyAssetId, LoadedAssetId.ToString())
					 ->SetVector(SpellsKeysForReplication::EnemyLocation, SpawnLocation)
				));

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
				if (PhotonCloudObject->IsDebugLogOn())
				{
					UE_LOG(LogTemp, Log, TEXT("Client Spawning enemy : ID: %d, Hash: %lld"), EnemyUniqueId, EnemyCharacter->HashedName);
				}
			}

			EnemyCharacter->FinishSpawning(SpawnTransform);
			EnemyCharacter->bInitialSync = true;
		}
		else
		{
			SpawnedEnemies.Remove(EnemyUniqueId);
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
	UPhotonJSON* PickablesJSON = UPhotonJSON::Create(this);
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

		const int32 PickableRandomClass = FMath::RandRange(0, SpawnPickableClasses.Num() - 1);
		ASPickableBase* Pickable = World->SpawnActor<ASPickableBase>(
			SpawnPickableClasses[PickableRandomClass],
			RandomLocation,
			FRotator::ZeroRotator);

		if (IsValid(Pickable))
		{
			Pickable->PickableUniqueId = ++CurrentPickableNetIdCount;
			SpawnedPickables.Add(Pickable->PickableUniqueId, Pickable);
			// Add pickable to the JSON to replicate with the p_{id} as key, location and class used
			PickablesJSON->Set_JSON_Object(
				SpellsKeysForReplication::PickablesPrefix + FString::FromInt(Pickable->PickableUniqueId),
				UPhotonJSON::Create(this)
				 ->SetVector(SpellsKeysForReplication::PickableLocation, RandomLocation)
				 ->SetShort(SpellsKeysForReplication::PickableClassId, PickableRandomClass));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Error spawning pickable of class %s"), *SpawnPickableClasses[PickableRandomClass]->GetName());
		}
	}

	// Send initial info for the spawned pickables
	if (PickablesJSON->GetCount() > 0)
	{
		PhotonCloudObject->AddCustomRoomProperties(PickablesJSON);
	}
}

ASPickableBase* ASpellsGameModeBase::MasterSpawnPickable(TSubclassOf<ASPickableBase> SpawnPickableClass, const FVector& SpawnLocation)
{
	ASPickableBase* Pickable = GetWorld()->SpawnActor<ASPickableBase>(SpawnPickableClass, SpawnLocation, FRotator::ZeroRotator);
	const FString ClassName = FStringClassReference(SpawnPickableClass).ToString();
	if (IsValid(Pickable))
	{
		Pickable->PickableUniqueId = ++CurrentPickableNetIdCount;
		SpawnedPickables.Add(Pickable->PickableUniqueId, Pickable);

		// Add pickable to the JSON to replicate with the p_{id} as key, location and class used
		PhotonCloudObject->AddCustomRoomProperties(UPhotonJSON::Create(this)->Set_JSON_Object(
			SpellsKeysForReplication::PickablesPrefix + FString::FromInt(Pickable->PickableUniqueId),
			UPhotonJSON::Create(this)
			 ->SetVector(SpellsKeysForReplication::PickableLocation, SpawnLocation)
			 ->SetString(SpellsKeysForReplication::PickableClassName, ClassName)));

		return Pickable;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Error spawning pickable of class %s"), *ClassName);

	return nullptr;
}

void ASpellsGameModeBase::KillAllEnemies()
{
#if !UE_BUILD_SHIPPING
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	for (const TPair<int32, ASAICharacter*> Enemy: SpawnedEnemies)
	{
		Enemy.Value->GetAttributesComponent()->Kill(PlayerController);
	}

	SpawnedEnemies.Empty();
#endif
}

void ASpellsGameModeBase::OnReceivedActorLocationRotation(const int64& HashedName, const FVector& NewLocation, const FRotator& NewRotation)
{
	UObject* Obj = PhotonCloudObject->GetSerializedObject(HashedName);
	if (ASAICharacter* EnemyCharacter = Cast<ASAICharacter>(Obj))
	{
		EnemyCharacter->OnReceivedActorLocationRotation(NewLocation, NewRotation);
	}
}

void ASpellsGameModeBase::OnReceivedActorData(const int64& InHashedName, UPhotonJSON* ActorData)
{
	if (!IsValid(ActorData))
	{
		return;
	}

	TArray<FString> Keys;
	ActorData->Data->Values.GetKeys(Keys);
	for (const FString& Key : Keys)
	{
		if (Key[0] == 'e')
		{
			static const int32 EnemiesActionEventStringLen = FCString::Strlen(SpellsKeysForReplication::EnemiesActionsEventPrefix);
			if (Key.Mid(0, EnemiesActionEventStringLen).Equals(SpellsKeysForReplication::EnemiesActionsEventPrefix))
			{
				const int32 EnemyUniqueId = FCString::Atoi(*Key.Mid(EnemiesActionEventStringLen, Key.Len() - EnemiesActionEventStringLen));
				if (const ASAICharacter* AICharacter = Cast<ASAICharacter>(FindMasterControlledActor(EnemyUniqueId)))
				{
					UPhotonJSON* PackedActionEventJSON = ActorData->Get_JSON_Object(Key);
					if (PhotonCloudObject->IsDebugLogOn())
					{
						UE_LOG(LogTemp, Log, TEXT("Receiving AI Action Event %s"), *PackedActionEventJSON->GetJSONasString());
					}
					AICharacter->GetActionsComponent()->OnActionsPropertiesChanged(PackedActionEventJSON);
				}
			}
		}
	}
}

void ASpellsGameModeBase::ClientSpawnEnemy(int32 EnemyUniqueId, const UPhotonJSON* EnemyJSON)
{
	const FPrimaryAssetId EnemyPrimaryAssetId(EnemyJSON->GetString(SpellsKeysForReplication::EnemyAssetId));
	const FStreamableDelegate AssetsLoadedCallback = FStreamableDelegate::CreateUObject(this, 
		&ASpellsGameModeBase::OnEnemyAssetsLoaded,
		EnemyPrimaryAssetId,
		EnemyJSON->GetVector(SpellsKeysForReplication::EnemyLocation), 
		EnemyUniqueId);

	UAssetManager::GetIfValid()->LoadPrimaryAsset(EnemyPrimaryAssetId, TArray<FName>(), AssetsLoadedCallback);
}

void ASpellsGameModeBase::OnNewMaster_Implementation(int32 OldMasterPlayerNumber, int32 NewMasterPlayerNumber)
{
	if (PhotonCloudObject->AmIMaster())
	{
		OnMasterControlGameMode();
	}
}

void ASpellsGameModeBase::ReplicateCharacterPickedSomething(ASCharacter* PickerCharacter, ASPickableBase* Pickable, bool bRemoveRoomProperty)
{
	if (IsValid(Pickable) && PickerCharacter->PlayerNumber == PhotonCloudObject->GetPlayerNumber())
	{
		// Only the current player needs to send the replication of the event
		UPhotonJSON* PickedEvent = UPhotonJSON::Create(this)->Set_JSON_Object(
				SpellsKeysForReplication::PickableEventPrefix + FString::FromInt(Pickable->PickableUniqueId),
				UPhotonJSON::Create(this)
					->SetBoolean(SpellsKeysForReplication::PickableIsPicked, true)
					->SetInteger(SpellsKeysForReplication::PickablePicker, PickerCharacter->PlayerNumber));
		PhotonCloudObject->AddCustomRoomProperties(PickedEvent);
		if (bRemoveRoomProperty)
		{
			// don't create this pickable for new players
			PhotonCloudObject->RemoveCustomRoomProperties(TArray<FString>{
				SpellsKeysForReplication::PickablesPrefix + FString::FromInt(Pickable->PickableUniqueId)
			});
		}
	}
}

void ASpellsGameModeBase::OnActorKilled(AActor* Killed, AActor* Killer)
{
	if (const ASCharacter* KilledCharacter = Cast<ASCharacter>(Killed))
	{
		if (APlayerController* PlayerController = KilledCharacter->GetController<APlayerController>())
		{
			FTimerHandle SpawnPlayerTimerHandle;
			FTimerDelegate SpawnPlayerTimerDelegate;
			SpawnPlayerTimerDelegate.BindUObject(this, &ASpellsGameModeBase::RestartPlayer, static_cast<AController*>(PlayerController));
			KilledCharacter->ActionsComponent->RemoveAllActions();
			PlayerController->UnPossess();
			PhotonCloudObject->RemovePlayerCustomProperties(ASCharacter::AllPhotonPlayerProperties);
			GetWorldTimerManager().SetTimer(SpawnPlayerTimerHandle, SpawnPlayerTimerDelegate, RespawnPlayerTime, false);
		}
		
		PhotonCloudObject->JoinedCharacters.Remove(KilledCharacter->PlayerNumber);
		Killed->SetLifeSpan(RespawnPlayerTime + 0.6f);
	}
	else if (const ASAICharacter* KilledAICharacter = Cast<ASAICharacter>(Killed))
	{
		if (PhotonCloudObject->AmIMaster())
		{
			const FString AIUniqueIdStr = FString::FromInt(KilledAICharacter->AIUniqueId);
			// Remove all persistent keys from the room props for this enemy
			PhotonCloudObject->RemoveCustomRoomProperties(TArray<FString>{
				SpellsKeysForReplication::EnemiesPrefix + AIUniqueIdStr,
				SpellsKeysForReplication::EnemiesActionsPrefix + AIUniqueIdStr,
				SpellsKeysForReplication::EnemiesAttributesPrefix + AIUniqueIdStr,
				SpellsKeysForReplication::EnemiesTargetActorPrefix + AIUniqueIdStr
			});
		}

		SpawnedEnemies.Remove(KilledAICharacter->AIUniqueId);
		PhotonCloudObject->UnregisterObservedMechanicsActor(KilledAICharacter->HashedName);
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

AActor* ASpellsGameModeBase::FindMasterControlledActor(int32 ActorUniqueId)
{
	return SpawnedEnemies.Contains(ActorUniqueId)
			? SpawnedEnemies[ActorUniqueId]
			: nullptr;
}

ASPickableBase* ASpellsGameModeBase::FindMasterControlledPickable(int32 PickableUniqueId)
{
	if (SpawnedPickables.Contains(PickableUniqueId))
	{
		return SpawnedPickables[PickableUniqueId];
	}

	// TODO add other elements like the items and pickables
	UE_LOG(LogTemp, Warning, TEXT("This pickable unique id is not mapped: %d"), PickableUniqueId);

	return nullptr;
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

ASPickableBase* ASpellsGameModeBase::ClientSpawnPickable(int32 PickableUniqueId, const UPhotonJSON* PickableJSON)
{
	if (IsValid(PickableJSON))
	{
		const int32 PickableClassIndex = PickableJSON->Contains(SpellsKeysForReplication::PickableClassId) 
					? PickableJSON->GetInteger(SpellsKeysForReplication::PickableClassId)
					: -1;
		UClass* PickableClass = nullptr;
		if (SpawnPickableClasses.IsValidIndex(PickableClassIndex))
		{
			PickableClass = SpawnPickableClasses[PickableClassIndex];
		} else if (PickableJSON->Contains(SpellsKeysForReplication::PickableClassName))
		{
			PickableClass = UPhotonCloudAPIBPLibrary::GetUClassFromString(
							PickableJSON->GetString(SpellsKeysForReplication::PickableClassName));
		}
		
		if (PickableClass && PickableClass->IsChildOf(ASPickableBase::StaticClass()))
		{
			ASPickableBase* Pickable = GetWorld()->SpawnActor<ASPickableBase>(
				PickableClass,
				PickableJSON->GetVector(SpellsKeysForReplication::PickableLocation),
			FRotator::ZeroRotator);
			
			if (IsValid(Pickable))
			{
				Pickable->PickableUniqueId = PickableUniqueId;
				CurrentPickableNetIdCount = FMath::Max(PickableUniqueId, CurrentPickableNetIdCount);
				SpawnedPickables.Add(PickableUniqueId, Pickable);
				return Pickable;
			}
		}
	}

	return nullptr;
}

void ASpellsGameModeBase::OnMasterControlGameMode()
{
	if (!PhotonCloudObject->AmIMaster()) // only master spawns items and enemies
	{
		return;
	}

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

	for (const TTuple<int32, ASAICharacter*>& EnemyPair : SpawnedEnemies)
	{
		EnemyPair.Value->OnMasterChanged();
	}
}

void ASpellsGameModeBase::OnClientRebuildGameMode()
{
	if (PhotonCloudObject->AmIMaster())
	{
		return;
	}

	OnCustomRoomPropertiesChanged(PhotonCloudObject->GetCustomRoomProperties());
}
