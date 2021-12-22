// Spells - xixgames - juaxix - 2021/2022

#include "Gameplay/SpellsGameStateBase.h"

/// Unreal includes
#include "GameFramework/GameStateBase.h" 
#include "GameFramework/PlayerState.h"
#include "Player/SCharacter.h"

TArray<ASCharacter*> ASpellsGameStateBase::GetAllPlayers() const
{
	TArray<ASCharacter*> Characters;
	Characters.Reserve(PlayerArray.Num());
	for (const APlayerState* Player : PlayerArray)
	{
		if (ASCharacter* Character = Cast<ASCharacter>(Player->GetPawn()))
		{
			Characters.Emplace(Character);
		}
	}

	return Characters;
}
