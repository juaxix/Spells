// Spells - xixgames - juaxix - 2021/2022

#include "Player/SPlayerState.h"

// Spells includes
#include "Gameplay/SSaveGame.h"

void ASPlayerState::GrantCredits(int32 Delta)
{
	if (Delta > 0)
	{
		Credits += Delta;
		OnCreditsChanged.Broadcast(this, Credits, Delta);
	}
}

bool ASPlayerState::ConsumeCredits(int32 Delta)
{
	if (Delta <= 0 || Credits < Delta)
	{
		return false;
	}

	Credits -= Delta;
	OnCreditsChanged.Broadcast(this, Credits, Delta);

	return true;
}

void ASPlayerState::SavePlayerState_Implementation(USSaveGame* SaveGame)
{
	if (IsValid(SaveGame))
	{
		SaveGame->Credits = Credits;
	}
}

void ASPlayerState::LoadPlayerState_Implementation(USSaveGame* SaveGame)
{
	if (SaveGame)
	{
		Credits = SaveGame->Credits;
	}
}