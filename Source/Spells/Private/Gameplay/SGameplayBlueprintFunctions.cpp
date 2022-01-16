// Spells - xixgames - juaxix - 2021/2022

#include "Gameplay/SGameplayBlueprintFunctions.h"

// Spells includes
#include "Gameplay/SAttributesComponent.h"
#include "Player/SCharacter.h"

// Cheats : not available in final builds (shipping)
static TAutoConsoleVariable<float> Spells_CVarApplyDamageForce(TEXT("spells.HitImpulseForce"), 300000.0f, TEXT("Force to apply when Applying Damage using the gameplay BP function"), ECVF_Cheat);

bool USGameplayBlueprintFunctions::ApplyDamage(AActor* DamageCauser, AActor* TargetActor, float DamageAmount, const FHitResult& HitResult)
{
	if (USAttributesComponent* AttributesComponent = USAttributesComponent::GetAttributesComponent(TargetActor))
	{
		if (const ASCharacter* Character = Cast<ASCharacter>(DamageCauser))
		{
			Character->AttributesComponent->ApplyManaChange(Character->DamageDoneToManaFraction * DamageAmount, TargetActor, HitResult);
		}

		if (AttributesComponent->ApplyHealthChange(-DamageAmount, DamageCauser, HitResult))
		{
			if (!AttributesComponent->IsAlive())
			{
				UPrimitiveComponent* HitComp = HitResult.GetComponent();
				if (HitComp && HitComp->IsSimulatingPhysics(HitResult.BoneName))
				{
					const FVector Direction = (HitResult.TraceEnd - HitResult.TraceStart).GetSafeNormal();
					HitComp->AddImpulseAtLocation(Direction * Spells_CVarApplyDamageForce.GetValueOnAnyThread(), HitResult.ImpactPoint, HitResult.BoneName);
				}
			}

			return true;
		}
	}

	return false;
}
