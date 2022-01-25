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
	USMagicProjectileAttack_Action();

	virtual void StartAction_Implementation(AActor* Instigator) override;

	virtual void StopAction_Implementation(AActor* Instigator) override
	{
		Super::StopAction_Implementation(Instigator);
		bRepeatingAction = false;
	}

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override
	{
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);
		DOREPLIFETIME(USMagicProjectileAttack_Action, Character);
	}

	virtual void ReceiveAnimationNotif_Implementation() override;

protected:
	void DoMagicalAttack();

	UPROPERTY(EditAnywhere, Category = "Spells|Attack")
	TSubclassOf<ASMagicProjectile> AttackProjectileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spells|Animation")
	FName SkeletalSocketName = TEXT("Muzzle_01");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spells|Animation")
	UAnimMontage* AnimMontage = nullptr;

	UPROPERTY(Replicated, VisibleInstanceOnly, BlueprintReadOnly, Category = "Spells|Setup")
	ACharacter* Character = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spells|Animation")
	uint8 bIsRepeatedAttack:1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spells|Debug")
	bool bDebugMode = false;

	static FCollisionObjectQueryParams ObjParams;
private:
	UPROPERTY(Transient)
	USkeletalMeshSocket const* SkeletalSocket = nullptr;

	uint8 bRepeatingAction:1;
};
