// Spells - xixgames - juaxix - 2021/2022

#include "Player/SAnimInstance.h"

// Unreal includes
#include "GameplayTagContainer.h"
#include "Gameplay/SActionsComponent.h"

void USAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (const AActor* OwningActor = GetOwningActor())
	{
		ActionsComponent = Cast<USActionsComponent>(OwningActor->GetComponentByClass(USActionsComponent::StaticClass()));
	}
	
}

void USAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	static FGameplayTag StunnedTag = FGameplayTag::RequestGameplayTag("Spells.Stunned");
	if (ActionsComponent)
	{
		bIsStunned = ActionsComponent->ActiveGameplayTags.HasTag(StunnedTag);
	}
}
