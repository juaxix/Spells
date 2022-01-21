// Spells - xixgames - juaxix - 2021/2022

#pragma once

// Unreal includes
#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "GameFramework/GameModeBase.h"

#include "SpellsGameModeBase.generated.h"

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
	USEnemyDataAsset* EnemyData;

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

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	virtual void StartPlay() override;

	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

	UFUNCTION(BlueprintCallable, Category = "Spells|Gameplay")
	void OnActorKilled(AActor* Killed, AActor* Killer);

	UFUNCTION(BlueprintCallable, Category = "Spells|SaveGame")
	void WriteSaveGame();

	void LoadSaveGame();

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
	
	void SpawnEnemyTimerElapsed();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spells|Enemies Spawning", meta = (AllowPrivateAccess = "true"))
	float SpawnEnemyTimerInterval = 30.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spells|Enemies Spawning", meta = (AllowPrivateAccess = "true"))
	UEnvQuery* SpawnEnemyQuery = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spells|Enemies Spawning", meta = (AllowPrivateAccess = "true", Tooltip = "Curve of enemy spawns over time"))
	UCurveFloat* SpawnEnemyInTimeCurve = nullptr;
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Spells|Enemies Spawning", meta = (AllowPrivateAccess = "true"))
	TArray<ASAICharacter*> SpawnedEnemies;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spells|Pickables", meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<class ASPickableBase>> SpawnPickableClasses;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spells|Pickables", meta = (AllowPrivateAccess = "true"))
	UEnvQuery* SpawnPickableQuery = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spells|Pickables", meta = (AllowPrivateAccess = "true"))
	int32 SpawnPickableLimit = 9;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spells|Pickables", meta = (AllowPrivateAccess = "true", Tooltip = "Min Separation of the items"))
	float SpawnPickableMinDistance = 500.0f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Spells|Pickables", meta = (AllowPrivateAccess = "true"))
	TArray<ASPickableBase*> SpawnedPickables;

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

	FTimerHandle TimerHandle_SpawnEnemies;
};
