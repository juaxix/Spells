// Spells - xixgames - juaxix - 2021/2022

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "SAIController.generated.h"

/**
 * Class to control our minions
 */
UCLASS()
class SPELLS_API ASAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
	UBehaviorTree* DefaultBehaviorTree = nullptr;

	uint8 MoveToLocationBBKeyID = 0;
	uint8 MoveToActorBBKeyID = 0;
};
