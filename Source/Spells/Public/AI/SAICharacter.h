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

class USActionsComponent;
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

	UFUNCTION()
	void OnHealthChanged(AActor* AttackerInstigatorActor, USAttributesComponent* AttributeComponent, float NewHealth, float Delta, const FHitResult& Hit);

public:
	UFUNCTION(BlueprintPure, Category = "Spells|AI|Attack") FORCEINLINE
	FVector GetMuzzleLocation() const { return MuzzleSocket->GetSocketLocation(GetMesh()); }

	UFUNCTION(BlueprintPure, Category = "Spells|Player|Attributes") FORCEINLINE
	USAttributesComponent* GetAttributesComponent() const { return AttributesComponent; }

	UFUNCTION(BlueprintPure, Category = "Spells|Player|Actions") FORCEINLINE
	USActionsComponent* GetActionsComponent() const { return ActionsComponent; }

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Projectile")
	float ProjectileDamage = 10.0f;

protected:
	UPROPERTY(Transient)
	USkeletalMeshSocket const* MuzzleSocket = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UPawnSensingComponent* PawnSensingComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	USAttributesComponent* AttributesComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	USActionsComponent* ActionsComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (ClampMin=1.0f, ClampMax=100.0f, UIMin=1.0f, UIMax=100.0f))
	float DestroyAfterKillSeconds = 9.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug", meta = (AllowPrivateAccess = true))
	uint8 bDebug:1;
};
