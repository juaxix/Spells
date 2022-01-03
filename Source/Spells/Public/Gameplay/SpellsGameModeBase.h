// Spells - xixgames - juaxix - 2021/2022

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "GameFramework/GameModeBase.h"

#include "SpellsGameModeBase.generated.h"

class ASAICharacter;
class UEnvQuery;

UCLASS()
class SPELLS_API ASpellsGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	ASpellsGameModeBase();
	
	virtual void StartPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Spells|Gameplay")
	void OnActorKilled(AActor* Killed, AActor* Killer);

protected:
	UFUNCTION(BlueprintPure, Category = "Spells|Bots")
	bool CanSpawnBot() const;

	UFUNCTION()
	void OnSpawnBotEQ_Completed(class UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);

	UFUNCTION()
	void OnPickableSpawnQueryCompleted(class UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);

	/// only used for debugging
	UFUNCTION(Exec, Category = "Spells|Bots")
	void KillAllBots();
	
	void SpawnBotTimerElapsed();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spells|Bots Spawning", meta = (AllowPrivateAccess = true))
	float SpawnBotTimerInterval = 30.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spells|Bots Spawning", meta = (AllowPrivateAccess = true))
	UEnvQuery* SpawnBotQuery = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spells|Bots Spawning", meta = (AllowPrivateAccess = true, Tooltip = "Curve of bot spawns over time"))
	UCurveFloat* SpawnBotInTimeCurve = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spells|Bots Spawning", meta = (AllowPrivateAccess = true))
	TSubclassOf<ASAICharacter> SpawnBotClass;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Spells|Bots Spawning", meta = (AllowPrivateAccess = true))
	TArray<ASAICharacter*> SpawnedBots;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spells|Pickables", meta = (AllowPrivateAccess = true))
	TArray<TSubclassOf<class ASPickableBase>> SpawnPickableClasses;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spells|Pickables", meta = (AllowPrivateAccess = true))
	UEnvQuery* SpawnPickableQuery = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spells|Pickables", meta = (AllowPrivateAccess = true))
	int32 SpawnPickableLimit = 9;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spells|Pickables", meta = (AllowPrivateAccess = true, Tooltip = "Min Separation of the items"))
	float SpawnPickableMinDistance = 500.0f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Spells|Pickables", meta = (AllowPrivateAccess = true))
	TArray<ASPickableBase*> SpawnedPickables;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spells|Character Spawning", meta = (AllowPrivateAccess = true))
	float RespawnPlayerTime = 6.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spells|Credits", meta = (AllowPrivateAccess = true))
	int32 CreditsGrantedPerKill = 20;
	
	FTimerHandle TimerHandle_SpawnBots;
};
