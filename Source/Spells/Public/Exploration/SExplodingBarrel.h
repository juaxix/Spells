// Spells - xixgames - juaxix - 2021/2022

#pragma once

// Unreal includes
#include "CoreMinimal.h"
#include "PhotonCloudObject.h"
#include "Engine/StaticMeshActor.h"

#include "SExplodingBarrel.generated.h"

class UPhotonReplicator;
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
	virtual void PostInitializeComponents() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintNativeEvent, Category = "Spells|Exploding Barrel")
	void OnBarrelHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void OnPlayerEnterSyncBarrel(int32 InPlayerNumber, FString InPlayerName, bool InIsLocalPlayer);

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Spells|Explosion")
	URadialForceComponent* RadialForceComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Spells|Photon Cloud")
	UPhotonReplicator* PhotonReplicatorComponent = nullptr;
};
