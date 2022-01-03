// Spells - xixgames - juaxix - 2021/2022

#include "Player/SPlayerState.h"

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
