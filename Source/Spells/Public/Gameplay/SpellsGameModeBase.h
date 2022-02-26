// Spells - xixgames - juaxix - 2021/2022

#pragma once

// Unreal includes
#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "GameFramework/GameModeBase.h"

#include "SpellsGameModeBase.generated.h"

class ASCharacter;
class ASPickableBase;
class UPhotonJSON;
class USPhotonCloudObject;
class ASAICharacter;
class UDataTable;
class USEnemyDataAsset;
class UEnvQuery;
class USSaveGame;

/**
 * Data for spawning enemies (NPC)
 */
USTRUCT(BlueprintType)
struct FSEnemyInfoRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	FPrimaryAssetId EnemyAssetId;

	/** Relative change to use this monster entry to be spawned */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn")
	float Weigth = 1.0f;

	/** Points required by the GameMode to spawn this enemy unit */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn")
	float SpawnCost = 5.0f;

	/** Amount of credits awarded to the killer of this enemy */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay")
	float KillCreditsReward = 10.0f;
};


UCLASS()
class SPELLS_API ASpellsGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	ASpellsGameModeBase();

	virtual void RestartPlayer(AController* NewPlayer) override;

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	virtual void StartPlay() override;

	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

	UFUNCTION(BlueprintNativeEvent, Category = "Spells|Photon Cloud")
	void OnCustomRoomPropertiesChanged(UPhotonJSON* RoomPropertiesJSON);

	UFUNCTION(BlueprintNativeEvent, Category = "Spells|Photon Cloud")
	void OnNewMaster(int32 OldMasterPlayerNumber, int32 NewMasterPlayerNumber);

	UFUNCTION(BlueprintCallable, Category = "Spells|Photon Cloud")
	void ReplicateCharacterPickedSomething(ASCharacter* PickerCharacter, ASPickableBase* Pickable, bool bRemoveRoomProperty = false);

	UFUNCTION(BlueprintCallable, Category = "Spells|Photon Cloud")
	ASPickableBase* MasterSpawnPickable(TSubclassOf<ASPickableBase> SpawnPickableClass, const FVector& SpawnLocation);

	UFUNCTION(BlueprintCallable, Category = "Spells|Gameplay")
	void OnActorKilled(AActor* Killed, AActor* Killer);

	UFUNCTION(BlueprintCallable, Category = "Spells|SaveGame")
	void WriteSaveGame();

	UFUNCTION(BlueprintCallable, Category = "Spells|SaveGame")
	void LoadSaveGame();

	UFUNCTION(BlueprintPure, Category = "Spells|SaveGame")
	AActor* FindMasterControlledActor(int32 ActorUniqueId);

	UFUNCTION(BlueprintPure, Category = "Spells|SaveGame")
	ASPickableBase* FindMasterControlledPickable(int32 PickableUniqueId);

	UFUNCTION(BlueprintPure, Category = "Spells|Enemies")
	const TMap<int32, ASAICharacter*>& GetEnemies() const{ return SpawnedEnemies; }

	void OnReceivedActorLocationRotation(const int64& HashedName, const FVector& NewLocation, const FRotator& NewRotation);

	void OnReceivedActorData(const int64& InHashedName, UPhotonJSON* ActorData);

protected:
	UFUNCTION(BlueprintPure, Category = "Spells|Enemies")
	bool CanSpawnEnemy() const;

	UFUNCTION()
	void OnSpawnEnemyEQ_Completed(class UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);

	UFUNCTION()
	void OnPickableSpawnQueryCompleted(class UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);

	/// only used for debugging
	UFUNCTION(Exec, Category = "Spells|Enemies")
	void KillAllEnemies();

protected:

	bool OnCustomRoomEnemiesPropertyChanged(const FString& EnemyPropertyName, UPhotonJSON* RoomEnemyPropertyJSON);

	void ClientSpawnEnemy(int32 EnemyUniqueId, const UPhotonJSON* EnemyJSON);

	void OnEnemyAssetsLoaded(FPrimaryAssetId LoadedAssetId, FVector SpawnLocation, int32 EnemyUniqueId);

	void SpawnEnemyTimerElapsed();

	ASPickableBase*  ClientSpawnPickable(int32 PickableUniqueId, const UPhotonJSON* PickableJSON);

	void OnMasterControlGameMode();

	void OnClientRebuildGameMode();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spells|Enemies Spawning", meta = (AllowPrivateAccess = "true"))
	float SpawnEnemyTimerInterval = 30.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spells|Enemies Spawning", meta = (AllowPrivateAccess = "true"))
	UEnvQuery* SpawnEnemyQuery = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spells|Enemies Spawning", meta = (AllowPrivateAccess = "true", Tooltip = "Curve of enemy spawns over time"))
	UCurveFloat* SpawnEnemyInTimeCurve = nullptr;
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Spells|Enemies Spawning", meta = (AllowPrivateAccess = "true"))
	TMap<int32, ASAICharacter*> SpawnedEnemies;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spells|Pickables", meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<ASPickableBase>> SpawnPickableClasses;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spells|Pickables", meta = (AllowPrivateAccess = "true"))
	UEnvQuery* SpawnPickableQuery = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spells|Pickables", meta = (AllowPrivateAccess = "true"))
	int32 SpawnPickableLimit = 9;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spells|Pickables", meta = (AllowPrivateAccess = "true", Tooltip = "Min Separation of the items"))
	float SpawnPickableMinDistance = 500.0f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Spells|Pickables", meta = (AllowPrivateAccess = "true"))
	TMap<int32, ASPickableBase*> SpawnedPickables;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spells|Character Spawning", meta = (AllowPrivateAccess = "true"))
	float RespawnPlayerTime = 6.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spells|Credits", meta = (AllowPrivateAccess = "true"))
	int32 CreditsGrantedPerKill = 20;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Spells|SaveGame", meta = (AllowPrivateAccess = "true"))
	USSaveGame* CurrentSaveGame;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spells|SaveGame", meta = (AllowPrivateAccess = "true"))
	FString SaveGameSlotName = TEXT("Spells");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spells|Enemies Spawning")
	UDataTable* EnemiesDataTable = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Spells|Photon Cloud")
	USPhotonCloudObject* PhotonCloudObject = nullptr;

	int32 CurrentActorNetIdCount = -1;
	int32 CurrentEnemyNetIdCount = -1;
	int32 CurrentPickableNetIdCount = -1;

	FTimerHandle TimerHandle_SpawnEnemies;
};
