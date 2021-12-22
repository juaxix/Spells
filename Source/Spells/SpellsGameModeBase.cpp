// Spells - xixgames - juaxix - 2021/2022

#include "SpellsGameModeBase.h"

/// Unreal includes
#include "GameFramework/GameStateBase.h" 
#include "GameFramework/PlayerState.h"
#include "Player/SCharacter.h"

// @todo move to the game state (accessible by all)
TArray<ASCharacter*> ASpellsGameModeBase::GetAllPlayers() const
{
	TArray<ASCharacter*> Characters;
	Characters.Reserve(GameState->PlayerArray.Num());
	for (const APlayerState* Player : GameState->PlayerArray)
	{
		if (ASCharacter* Character = Cast<ASCharacter>(Player->GetPawn()))
		{
			Characters.Emplace(Character);
		}
	}

	return Characters;
}
