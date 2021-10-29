// Spells - xixgames - juaxix - 2021

#include "Exploration/SExplodingBarrel.h"
#include "DrawDebugHelpers.h"
#include "Attacks/SMagicProjectile.h"
#include "PhysicsEngine/RadialForceComponent.h"

ASExplodingBarrel::ASExplodingBarrel()
{
	PrimaryActorTick.bCanEverTick = false;
	RadialForceComponent = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForce"));
	RadialForceComponent->SetupAttachment(RootComponent); // Root == StaticMeshComponent
	RadialForceComponent->SetAutoActivate(false);
	RadialForceComponent->Radius = 700.0f;
	RadialForceComponent->ImpulseStrength = 2000.0f;
	RadialForceComponent->bImpulseVelChange = true;
	RadialForceComponent->AddCollisionChannelToAffect(ECC_WorldDynamic);
	GetStaticMeshComponent()->SetSimulatePhysics(true);
}

void ASExplodingBarrel::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	GetStaticMeshComponent()->OnComponentHit.AddDynamic(this, &ASExplodingBarrel::OnBarrelHit);
}

void ASExplodingBarrel::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetStaticMeshComponent()->OnComponentHit.RemoveAll(this);
	Super::EndPlay(EndPlayReason);
}

void ASExplodingBarrel::OnBarrelHit_Implementation(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (const ASMagicProjectile* MagicProjectile = Cast<ASMagicProjectile>(OtherActor))
	{
		RadialForceComponent->FireImpulse();

		const FString DebugString = FString::Printf(TEXT("Hit from %s at %s"), MagicProjectile->GetInstigator() ? *MagicProjectile->GetInstigator()->GetName() : TEXT("Unknown"), *Hit.Location.ToString());
		DrawDebugString(GetWorld(), Hit.ImpactPoint, DebugString, nullptr, FColor::Yellow, 2.0f, true, 1);
	}
}
