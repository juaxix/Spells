// Spells - xixgames - juaxix - 2021

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SMagicProjectile.generated.h"

class UProjectileMovementComponent;
class USphereComponent;

UCLASS()
class SPELLS_API ASMagicProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	ASMagicProjectile();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Projectile")
	USphereComponent* SphereComponent = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Projectile")
	UProjectileMovementComponent* ProjectileMovementComponent = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "FX")
	UParticleSystemComponent* EffectComponent = nullptr;
};
