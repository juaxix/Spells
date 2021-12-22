// Spells - xixgames - juaxix - 2021/2022

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SpellsGameModeBase.generated.h"

class ASCharacter;

UCLASS()
class SPELLS_API ASpellsGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Spells|Game Mode")
	TArray<ASCharacter*> GetAllPlayers() const;
};
