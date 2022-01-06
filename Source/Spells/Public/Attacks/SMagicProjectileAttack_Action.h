// Spells - xixgames - juaxix - 2021/2022

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/SAction.h"
#include "SMagicProjectileAttack_Action.generated.h"

class ASMagicProjectile;


UCLASS()
class SPELLS_API USMagicProjectileAttack_Action : public USAction
{
	GENERATED_BODY()

public:
	USMagicProjectileAttack_Action()
		: bIsRepeatedAttack(false)
		, bRepeatedAction(false)
	{}

	virtual void StartAction_Implementation(AActor* Instigator) override;

	virtual void StopAction_Implementation(AActor* Instigator) override
	{
		Super::StopAction_Implementation(Instigator);
		bRepeatedAction = false;
	}

	virtual void ReceiveAnimationNotif_Implementation() override;

protected:
	bool DoMagicalAttack();

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

	uint8 bRepeatedAction:1;

	UPROPERTY(Transient)
	USkeletalMeshSocket const* SkeletalSocket = nullptr;
};
