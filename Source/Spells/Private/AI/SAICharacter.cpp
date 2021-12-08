// Spells - xixgames - juaxix - 2021/2022
#include "AI/SAICharacter.h"

namespace
{
	const FName SMUZZLE_NAME = TEXT("Muzzle_01");
}


ASAICharacter::ASAICharacter()
{
	PrimaryActorTick.bCanEverTick = false;

}


void ASAICharacter::BeginPlay()
{
	Super::BeginPlay();
	// cache muzzle
	MuzzleSocket = GetMesh()->GetSocketByName(SMUZZLE_NAME);
	
}
