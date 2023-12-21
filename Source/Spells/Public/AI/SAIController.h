// Spells - xixgames - juaxix - 2021/2022

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/Blackboard/BlackboardKey.h"
#include "SAIController.generated.h"

class USPhotonCloudObject;

namespace SpellsAIController
{
	const FName STARGET_ACTOR_BB_KEY = TEXT("TargetActor");
	const FName STARGET_LOCATION_BB_KEY = TEXT("TargetLocation");
	const FName SWITHIN_ATTACK_RANGE_BB_KEY = TEXT("bWithinAttackRange");
	const FName SWITHIN_ATTACK_SIGHT_BB_KEY = TEXT("bActorInLineOfSight");
}

/**
 * Class to control our minions
 */
UCLASS()
class SPELLS_API ASAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

	UFUNCTION(BlueprintCallable, Category = "Spells|AI|Target")
	bool SetCurrentTargetActor(AActor* InActor);

	UFUNCTION(BlueprintPure, Category = "Spells|AI|Target")
	AActor* GetCurrentTargetActor() const;

	UFUNCTION(BlueprintCallable, Category = "Spells|AI|Target")
	bool SetCurrentTargetLocation(const FVector& InLocation);

	UFUNCTION(BlueprintPure, Category = "Spells|AI|Target")
	FVector GetCurrentTargetLocation() const;

protected:
	UFUNCTION()
	void OnPhotonMasterPlayerChanged(int32 OldMasterPlayerNumber, int32 NewMasterPlayerNumber)
	{
		SetupBehavior();
	}

	void SetupBehavior();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spells|AI")
	UBehaviorTree* MasterBehaviorTree = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spells|AI")
	UBehaviorTree* ClientBehaviorTree = nullptr;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Spells|Photon Cloud")
	USPhotonCloudObject* PhotonCloudObject = nullptr;

	FBlackboard::FKey MoveToLocationBBKeyID;
	FBlackboard::FKey MoveToActorBBKeyID;
};
