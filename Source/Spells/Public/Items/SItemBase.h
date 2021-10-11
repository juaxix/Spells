// Spells - xixgames - juaxix - 2021

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gameplay/SInteractableInterface.h"
#include "SItemBase.generated.h"

UCLASS()
class SPELLS_API ASItemBase : public AActor, public ISInteractableInterface
{
	GENERATED_BODY()
	
public:	
	ASItemBase();

protected:
	UPROPERTY(Category = StaticMeshActor, VisibleAnywhere, BlueprintReadOnly, meta = (ExposeFunctionCategories = "Mesh,Rendering,Physics,Components|StaticMesh", AllowPrivateAccess = "true"))
	UStaticMeshComponent* BaseStaticMesh;

	UPROPERTY(Category = StaticMeshActor, VisibleAnywhere, BlueprintReadOnly, meta = (ExposeFunctionCategories = "Mesh,Rendering,Physics,Components|StaticMesh", AllowPrivateAccess = "true"))
	UStaticMeshComponent* LidStaticMesh;
};
