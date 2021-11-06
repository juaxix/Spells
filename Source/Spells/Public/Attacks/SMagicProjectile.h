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

	UFUNCTION(BlueprintNativeEvent, Category = "Spells|Projectile")
	void OnSphereActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION(BlueprintNativeEvent, Category = "Spells|Projectile")
	void OnSphereActorHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION(BlueprintImplementableEvent, Category = "Spells|Projectile")
	void OnProjectileStopped(const FHitResult& ImpactResult);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Projectile")
	USphereComponent* SphereComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Projectile")
	UProjectileMovementComponent* ProjectileMovementComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "FX")
	UParticleSystemComponent* EffectComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
	float Damage = 20.0f;
};
