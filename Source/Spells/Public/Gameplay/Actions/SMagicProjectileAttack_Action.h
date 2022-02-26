// Spells - xixgames - juaxix - 2021/2022

#pragma once

// Unreal includes
#include "CoreMinimal.h"

// Spells includes
#include "Gameplay/Actions/SAction.h"

#include "SMagicProjectileAttack_Action.generated.h"

class ASMagicProjectile;

UCLASS()
class SPELLS_API USMagicProjectileAttack_Action : public USAction
{
	GENERATED_BODY()

public:
	USMagicProjectileAttack_Action()
		: bIsRepeatedAttack(false)
		, bRepeatingAction(false)
	{
	}

	virtual void StartAction_Implementation(AActor* Instigator) override;

	virtual void StopAction_Implementation(AActor* Instigator) override
	{
		Super::StopAction_Implementation(Instigator);
		bRepeatingAction = false;
	}

	virtual void ReceiveAnimationNotif_Implementation() override;

	virtual void OnPropertiesChanged_Implementation(const UPhotonJSON* InActionJSON) override;

protected:
	void Owner_DoMagicalAttack();

	void SpawnMagicProjectile(const FVector& Location, const FRotator& Rotation);

	UPROPERTY(EditAnywhere, Category = "Spells|Attack")
	TSubclassOf<ASMagicProjectile> AttackProjectileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spells|Animation")
	FName SkeletalSocketName = TEXT("Muzzle_01");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spells|Animation")
	UAnimMontage* AnimMontage = nullptr;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Spells|Setup")
	ACharacter* Character = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spells|Animation")
	uint8 bIsRepeatedAttack:1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spells|Debug")
	bool bDebugMode = false;

private:
	UPROPERTY(Transient)
	USkeletalMeshSocket const* SkeletalSocket = nullptr;

	uint8 bRepeatingAction:1;
};
