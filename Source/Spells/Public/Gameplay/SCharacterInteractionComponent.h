// Spells - xixgames - juaxix - 2021/2022

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SCharacterInteractionComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPELLS_API USCharacterInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USCharacterInteractionComponent();

	virtual void PrimaryAction();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
	float MaxInteractionDistance = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
	FVector InteractionOffset = FVector(0.0f, 0.0f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
	float InteractionRadius = 30.0f;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup", meta = (AllowPrivateAccess = "true"))
	uint8 bDebugInteraction:1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup", meta = (AllowPrivateAccess = "true"))
	uint8 bCheckForWalls:1;

	FCollisionObjectQueryParams CollisionObjectQueryParams_StaticDynamic, CollisionObjectQueryParams_Static;
};
