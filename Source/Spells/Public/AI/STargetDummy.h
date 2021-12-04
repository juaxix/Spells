// Spells - xixgames - juaxix - 2021

#pragma once

/// Engine includes
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "STargetDummy.generated.h"

class USAttributesComponent;
UCLASS()
class SPELLS_API ASTargetDummy : public AActor
{
	GENERATED_BODY()
	
public:	
	ASTargetDummy();

protected:
	virtual void PostInitializeComponents() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void OnHealthChanged(AActor* InstigatorActor, USAttributesComponent* OwningAttributesComp, float NewHealth, float Delta, const FHitResult& Hit);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	USAttributesComponent* AttributesComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UStaticMeshComponent* StaticMeshComponent = nullptr;
};
