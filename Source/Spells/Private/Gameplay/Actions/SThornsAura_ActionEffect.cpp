// Spells - xixgames - juaxix - 2021/2022

#include "Gameplay/Actions/SThornsAura_ActionEffect.h"

// Spells includes
#include "Gameplay/SAttributesComponent.h"
#include "Gameplay/SGameplayBlueprintFunctions.h"

void USThornsAura_ActionEffect::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

	// Start listening for my health change to apply a fraction of the damage to the attacker player
	if (USAttributesComponent* AttributesComponent = 
		USAttributesComponent::GetAttributesComponent(GetOwningComponent()->GetOwner()))
	{
		AttributesComponent->OnHealthAttributeChanged.AddDynamic(this, &USThornsAura_ActionEffect::OnHealthChanged);
	}
}

void USThornsAura_ActionEffect::StopAction_Implementation(AActor* Instigator)
{
	Super::StopAction_Implementation(Instigator);

	// Stop listening for my health change
	if (USAttributesComponent* AttributesComponent = 
		USAttributesComponent::GetAttributesComponent(GetOwningComponent()->GetOwner()))
	{
		AttributesComponent->OnHealthAttributeChanged.RemoveDynamic(this, &USThornsAura_ActionEffect::OnHealthChanged);
	}
}

void USThornsAura_ActionEffect::OnHealthChanged(AActor* InstigatorActor, USAttributesComponent* OwningAttributesComp, float NewHealth, float Delta, const FHitResult& Hit)
{
	AActor* OwningActor = GetOwningComponent()->GetOwner();

	// is it damage (not healing)?
	if (Delta < 0.0f && OwningActor != InstigatorActor && IsValid(InstigatorActor))
	{
		int32 ThornsDamage = FMath::RoundToInt(Delta * DamageFraction);
		if (ThornsDamage == 0)
		{
			return;
		}

		FHitResult HitBack;
		HitBack.Location = InstigatorActor->GetActorLocation();

		// return back the damage as positive
		USGameplayBlueprintFunctions::ApplyDamage(OwningActor, InstigatorActor, FMath::Abs(ThornsDamage), HitBack);
	}
}


