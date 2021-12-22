// Spells - xixgames - juaxix - 2021/2022

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "SpellsGameStateBase.generated.h"

class ASCharacter;

UCLASS()
class SPELLS_API ASpellsGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Spells|Game State")
	TArray<ASCharacter*> GetAllPlayers() const;
};
