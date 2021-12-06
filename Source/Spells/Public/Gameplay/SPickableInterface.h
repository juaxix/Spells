// Spells - xixgames - juaxix - 2021/2022

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SPickableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USPickableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * An interface to define associations between players and pickable objects in the world
 */
class SPELLS_API ISPickableInterface
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Spells|Picking")
	void Pick(APawn* InstigatorPawn);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Spells|Picking")
	void Drop(const FTransform& DestinyTransform);
};
