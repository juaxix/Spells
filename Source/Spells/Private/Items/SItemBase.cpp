// Spells - xixgames - juaxix - 2021


#include "Items/SItemBase.h"

ASItemBase::ASItemBase()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = BaseStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseStaticMesh"));
	LidStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LidStaticMesh"));
	LidStaticMesh->SetupAttachment(RootComponent);
}
