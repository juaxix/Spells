// Spells - xixgames - juaxix - 2021
#include "AI/STargetDummy.h"

/// Unreal includes
#include "Components\StaticMeshComponent.h"

/// Spells includes
#include "Player/SAttributesComponent.h"

namespace
{
	const FName SMATERIAL_HIT_PARAMETER = TEXT("TimeToHit");
}

ASTargetDummy::ASTargetDummy()
{
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	AttributesComponent = CreateDefaultSubobject<USAttributesComponent>(TEXT("AttributesComponent"));
}

void ASTargetDummy::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (!HasAnyFlags(RF_DefaultSubObject|RF_ClassDefaultObject|RF_Transient) && 
		!AttributesComponent->OnHealthAttributeChanged.IsAlreadyBound(this, &ASTargetDummy::OnHealthChanged))
	{
		AttributesComponent->OnHealthAttributeChanged.AddDynamic(this, &ASTargetDummy::OnHealthChanged);
	}
}

void ASTargetDummy::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	AttributesComponent->OnHealthAttributeChanged.RemoveAll(this);
}

void ASTargetDummy::OnHealthChanged(AActor* InstigatorActor, USAttributesComponent* OwningAttributesComp, float NewHealth, float Delta, const FHitResult& Hit)
{
	if (Delta < 0.0f)
	{
		StaticMeshComponent->SetScalarParameterValueOnMaterials(SMATERIAL_HIT_PARAMETER, GetWorld()->TimeSeconds);
	}
}
