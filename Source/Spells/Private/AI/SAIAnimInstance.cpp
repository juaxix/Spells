// Spells - xixgames - juaxix - 2021/2022


#include "AI/SAIAnimInstance.h"

#include "AI/SAICharacter.h"

void USAIAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	const APawn* Pawn = TryGetPawnOwner();
	if (!IsValid(Pawn))
	{
		return;
	}

	if (const ASAICharacter* AICharacter = Cast<ASAICharacter>(Pawn))
	{
		Speed = AICharacter->GetCurrentSpeed();
	}
	else
	{
		Speed = Pawn->GetVelocity().Size();
	}
}
