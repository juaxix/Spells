// Spells - xixgames - juaxix - 2021

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "SExplodingBarrel.generated.h"

class URadialForceComponent;

/**
 * Exploding Barrels (or sometimes colloquially Explosive Barrels) can be encountered during exploration.
 * Attacking the barrel will detonate it, dealing AoE damage to nearby players.
 */
UCLASS()
class SPELLS_API ASExplodingBarrel : public AStaticMeshActor
{
	GENERATED_BODY()

public:
	ASExplodingBarrel();

protected:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintNativeEvent, Category = "Spells|Exploding Barrel")
	void OnBarrelHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Explosion")
	URadialForceComponent* RadialForceComponent = nullptr;

};
