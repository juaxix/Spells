// Spells - xixgames - juaxix - 2021/2022

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gameplay/SPickableInterface.h"

#include "SPickableBase.generated.h"

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

	UPROPERTY(Category = "Spells|Setup", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int32 CreditsCost = 0;

	UPROPERTY(Category = "Spells|Setup", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int32 CreditsGranted = 0;
};
