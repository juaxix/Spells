// Spells - xixgames - juaxix - 2021/2022

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SSaveGame.generated.h"

USTRUCT(BlueprintType)
struct FSActorSaveData
{
	GENERATED_BODY()

public:
	/* Name identifier of this actor */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spells|SaveGame")
	FString ActorName;

	/* For movable Actors: keep location,rotation and scale */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spells|SaveGame")
	FTransform Transform;

	/* Variables with the bSaveGame flag enabled (serialized) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spells|SaveGame")
	TArray<uint8> ByteData;
};

/**
 * Storing variables and actors of the game
 */
UCLASS(BlueprintType, Blueprintable)
class SPELLS_API USSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spells|SaveGame")
	int32 Credits = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spells|SaveGame")
	TArray<FSActorSaveData> SavedActors;
};
