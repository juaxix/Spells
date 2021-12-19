// Spells - xixgames - juaxix - 2021/2022

#pragma once

#include "CoreMinimal.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/Character.h"
#include "SAICharacter.generated.h"

/**
 * Pawn sensing is the basic (original) version of the AI perception
 */
class UPawnSensingComponent;

class USAttributesComponent;

UCLASS()
class SPELLS_API ASAICharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASAICharacter();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnPawnInSight(APawn* InPawn);

public:
	UFUNCTION(BlueprintPure, Category = "Spells|AI|Attack")
	FORCEINLINE FVector GetMuzzleLocation() const
	{
		return MuzzleSocket->GetSocketLocation(GetMesh());
	}

	UFUNCTION(BlueprintPure, Category = "Spells|Player|Attributes") FORCEINLINE
	USAttributesComponent* GetAttributesComponent() const {return AttributesComponent;}
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Projectile")
	float ProjectileDamage = 10.0f;

protected:
	UPROPERTY(Transient)
	USkeletalMeshSocket const* MuzzleSocket = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UPawnSensingComponent* PawnSensingComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	USAttributesComponent* AttributesComponent = nullptr;
};
