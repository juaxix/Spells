// Spells - xixgames - juaxix - 2021


#include "Exploration/SExplodingBarrel.h"

#include "Attacks/SMagicProjectile.h"
#include "PhysicsEngine/RadialForceComponent.h"

ASExplodingBarrel::ASExplodingBarrel()
{
	PrimaryActorTick.bCanEverTick = false;
	RadialForceComponent = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForce"));
	RadialForceComponent->SetupAttachment(RootComponent); // Root == StaticMeshComponent
	RadialForceComponent->Radius = 700.0f;
	RadialForceComponent->ImpulseStrength = 2000.0f;
	RadialForceComponent->bImpulseVelChange = true;
	
	GetStaticMeshComponent()->SetSimulatePhysics(true);
}

void ASExplodingBarrel::BeginPlay()
{
	Super::BeginPlay();
	GetStaticMeshComponent()->OnComponentHit.AddDynamic(this, &ASExplodingBarrel::OnBarrelHit);
}

void ASExplodingBarrel::OnBarrelHit_Implementation(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (ASMagicProjectile* MagicProjectile = Cast<ASMagicProjectile>(OtherActor))
	{
		RadialForceComponent->FireImpulse();
	}
}
