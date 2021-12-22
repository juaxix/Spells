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
	virtual void StartPlay() override;

protected:
	UFUNCTION(BlueprintPure, Category = "Spells|Bots")
	bool CanSpawnBot() const;

	UFUNCTION()
	void OnSpawnBotEQ_Completed(class UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);

	void SpawnBotTimerElapsed();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Bots Spawning", meta = (AllowPrivateAccess = true))
	float SpawnBotTimerInterval = 30.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Bots Spawning", meta = (AllowPrivateAccess = true))
	UEnvQuery* SpawnBotQuery = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Bots Spawning", meta = (AllowPrivateAccess = true, Tooltip = "Curve of bot spawns over time"))
	UCurveFloat* SpawnBotInTimeCurve = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Bots Spawning", meta = (AllowPrivateAccess = true))
	TSubclassOf<ASAICharacter> SpawnBotClass;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Bots Spawning", meta = (AllowPrivateAccess = true))
	TArray<ASAICharacter*> SpawnedBots;

	FTimerHandle TimerHandle_SpawnBots;
};
