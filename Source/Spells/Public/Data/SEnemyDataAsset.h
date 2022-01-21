// Spells - xixgames - juaxix - 2021/2022

#pragma once

// Unreal includes
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "SEnemyDataAsset.generated.h"

class USAction;

/**
 * Data used to define an Enemy
 */
UCLASS()
class SPELLS_API USEnemyDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()


public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn Info")
	TSubclassOf<AActor> EnemyClass;

	/** Actions/buffs to grant to this Enemy */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn Info")
	TArray<TSubclassOf<USAction>> Actions;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	UTexture2D* Icon = nullptr;
};
