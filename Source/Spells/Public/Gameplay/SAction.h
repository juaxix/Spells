// Spells - xixgames - juaxix - 2021/2022

#pragma once

// Engine includes
#include "CoreMinimal.h"
#include "SAction.generated.h"

/**
 * To create child classes as BPs for different actions used in the @see USActionComponent
 */
UCLASS(BlueprintType, Blueprintable)
class SPELLS_API USAction : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category = "Spells|Actions")
	void StartAction(AActor* Instigator);

	UFUNCTION(BlueprintNativeEvent, Category = "Spells|Actions")
	void StopAction(AActor* Instigator);
	
	virtual void ReceiveAnimationNotif(){}

	virtual UWorld* GetWorld() const override
	{
		if (UActorComponent* ActorComponent = Cast<UActorComponent>(GetOuter()))
		{
			return ActorComponent->GetWorld();
		}

		return nullptr;
	}

	/* Action nickname to start/stop without a reference to the object */
	UPROPERTY(EditDefaultsOnly, Category = "Spells|Actions")
	FName ActionName;
};
