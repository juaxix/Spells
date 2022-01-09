// Spells - xixgames - juaxix - 2021/2022

#pragma once

// Unreal includes
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"

#include "SMagicProjectile.generated.h"

class USActionEffect;
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
	void OnSphereActorHit_Implementation(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
	{
		OnSphereActorOverlap(HitComponent, OtherActor, OtherComp, 0, false, Hit);
	}

	UFUNCTION(BlueprintImplementableEvent, Category = "Spells|Projectile")
	void OnProjectileStopped(const FHitResult& ImpactResult);

	UFUNCTION(BlueprintCallable, Category = "Spells|Projectile")
	void SetProjectileDamage(float NewDamage) { Damage = NewDamage; }

	UFUNCTION(BlueprintCallable, Category = "Spells|Action Effects")
	void RemoveActionEffectClass(const TSubclassOf<USActionEffect>& ActionEffectClass);

	UFUNCTION(BlueprintCallable, Category = "Spells|Action Effects")
	void SetActionEffectClasses(const TArray<TSubclassOf<USActionEffect>>& InActionEffectClasses, bool bAppend = true);

protected:
	virtual void PostInitializeComponents() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Spells|Projectile")
	USphereComponent* SphereComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Spells|Projectile")
	UProjectileMovementComponent* ProjectileMovementComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Spells|FX")
	UParticleSystemComponent* EffectComponent = nullptr;

	UPROPERTY(EditAnywhere, Category = "Spells|Attack", meta = (Tooltip = "Action Effect list to apply on hit to the target"))
	TArray<TSubclassOf<USActionEffect>> ActionEffectClasses;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spells|Damage", meta = (AllowPrivateAccess = true))
	float Damage = 20.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spells|Damage", meta = (AllowPrivateAccess = true))
	FGameplayTag CounterSpellTag;
};
