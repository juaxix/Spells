// Spells - xixgames - juaxix - 2021

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SMagicProjectile.generated.h"

class UProjectileMovementComponent;
class USphereComponent;

namespace SpellsGame
{
	const FName MagicProjectileChannel = TEXT("MagicProjectile");
}

UCLASS()
class SPELLS_API ASMagicProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	ASMagicProjectile();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Projectile")
	USphereComponent* SphereComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Projectile")
	UProjectileMovementComponent* ProjectileMovementComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "FX")
	UParticleSystemComponent* EffectComponent = nullptr;
};
