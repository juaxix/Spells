// Spells - xixgames - juaxix - 2021/2022

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SInteractableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * An interface to define interactions between players and actors in the world
 */
class SPELLS_API ISInteractableInterface
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Spells|Interaction")
	void Interact(APawn* InstigatorPawn);

	// Called after an actor state was restored from a SaveGame file
	UFUNCTION(BlueprintNativeEvent, Category = "Spells|Interaction")
	void OnRestoredStateFromSaveGame();
};
