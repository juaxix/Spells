// Spells - xixgames - juaxix - 2021/2022

#pragma once

// Unreal includes
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"

// Spells includes
#include "Gameplay/SPickableInterface.h"

#include "SPickableBase.generated.h"

class USActionEffect;

UCLASS(BlueprintType, Blueprintable)
class SPELLS_API ASPickableBase : public AActor, public ISPickableInterface
{
	GENERATED_BODY()
	
public:	
	ASPickableBase()
	{
		PrimaryActorTick.bCanEverTick = false;
		RootComponent = BaseStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseStaticMesh"));
		
		DecorationStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DecorationStaticMesh"));
		DecorationStaticMesh->SetupAttachment(RootComponent);

		bReplicates = true;
	}

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override
	{
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);

		DOREPLIFETIME(ASPickableBase, CreditsCost);
		DOREPLIFETIME(ASPickableBase, CreditsGranted);
	}

protected:
	UPROPERTY(Category = "Spells|Setup", VisibleAnywhere, BlueprintReadOnly, meta = (ExposeFunctionCategories = "Mesh,Rendering,Physics,Components|StaticMesh", AllowPrivateAccess = "true"))
	UStaticMeshComponent* BaseStaticMesh = nullptr;

	UPROPERTY(Category = "Spells|Setup", VisibleAnywhere, BlueprintReadOnly, meta = (ExposeFunctionCategories = "Mesh,Rendering,Physics,Components|StaticMesh", AllowPrivateAccess = true))
	UStaticMeshComponent* DecorationStaticMesh = nullptr;

	UPROPERTY(Category = "Spells|Setup", EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* PickParticleSystem = nullptr;

	UPROPERTY(Category = "Spells|Setup", EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* DropParticleSystem = nullptr;

	UPROPERTY(Category = "Spells|Setup", EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USoundBase* PickSound = nullptr;

	UPROPERTY(Category = "Spells|Setup", EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USoundBase* DropSound = nullptr;

	UPROPERTY(Replicated, Category = "Spells|Setup", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int32 CreditsCost = 0;

	UPROPERTY(Replicated, Category = "Spells|Setup", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int32 CreditsGranted = 0;

	UPROPERTY(Category = "Spells|Setup", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true", Tooltip = "List of effects to be added to the character picking this item"))
	TArray<TSubclassOf<USActionEffect>> ActionEffectsGranted;

	UPROPERTY(Category = "Spells|Setup", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true", Tooltip = "List of effects to be removed from the character picking this item"))
	TArray<TSubclassOf<USActionEffect>> ActionEffectsRemoved;
};
