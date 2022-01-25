// Spells - xixgames - juaxix - 2021/2022

#pragma once

// Unreal includes
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"

// Spells includes
#include "Gameplay/SInteractableInterface.h"

#include "SItemBase.generated.h"

UCLASS()
class SPELLS_API ASItemBase : public AActor, public ISInteractableInterface
{
	GENERATED_BODY()
	
public:	
	ASItemBase();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override
	{
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
		DOREPLIFETIME(ASItemBase, CreditsCost);
		DOREPLIFETIME(ASItemBase, CreditsGranted);
	}

protected:
	UPROPERTY(Category = "Spells|Setup", VisibleAnywhere, BlueprintReadOnly, meta = (ExposeFunctionCategories = "Mesh,Rendering,Physics,Components|StaticMesh", AllowPrivateAccess = "true"))
	UStaticMeshComponent* BaseStaticMesh;

	UPROPERTY(Category = "Spells|Setup", VisibleAnywhere, BlueprintReadOnly, meta = (ExposeFunctionCategories = "Mesh,Rendering,Physics,Components|StaticMesh", AllowPrivateAccess = "true"))
	UStaticMeshComponent* LidStaticMesh;

	UPROPERTY(Replicated, Category = "Spells|Setup", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int32 CreditsGranted = 0;

	UPROPERTY(Replicated, Category = "Spells|Setup", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int32 CreditsCost = 0;
};
