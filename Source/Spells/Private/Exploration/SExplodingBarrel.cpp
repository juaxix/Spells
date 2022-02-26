// Spells - xixgames - juaxix - 2021/2022

#include "Exploration/SExplodingBarrel.h"

// Unreal includes
#include "DrawDebugHelpers.h"
#include "PhysicsEngine/RadialForceComponent.h"

// Photon includes
#include "PhotonReplicator.h"
#include "PhotonCloudAPIBPLibrary.h"
#include "PhotonCloudSubsystem.h"

// Spells includes
#include "Gameplay/Attacks/SMagicProjectile.h"

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

	PhotonReplicatorComponent = CreateDefaultSubobject<UPhotonReplicator>(TEXT("PhotonReplicator"));
	PhotonReplicatorComponent->OwnerRole = EPhotonReplicatorOwnerRoles::MASTER;
	PhotonReplicatorComponent->LocationReplication = EPhotonReplicateAxis::XYZ;
	PhotonReplicatorComponent->RotationReplication = EPhotonReplicateAxis::XYZ;
}

void ASExplodingBarrel::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	GetStaticMeshComponent()->OnComponentHit.AddDynamic(this, &ASExplodingBarrel::OnBarrelHit);
	if (PhotonReplicatorComponent->RequestOwnership())
	{
		PhotonReplicatorComponent->GetPhotonCloudRef()->OnPlayerJoinedRoom.AddDynamic(this, &ASExplodingBarrel::OnPlayerEnterSyncBarrel);
	}
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
		if (PhotonReplicatorComponent->IsMine())
		{
			RadialForceComponent->FireImpulse();
		}
		
		const FString DebugString = FString::Printf(TEXT("Hit from %s at %s"), MagicProjectile->GetInstigator() ? *MagicProjectile->GetInstigator()->GetName() : TEXT("Unknown"), *Hit.Location.ToString());
		DrawDebugString(GetWorld(), Hit.ImpactPoint, DebugString, nullptr, FColor::Yellow, 2.0f, true, 1);
	}
}

void ASExplodingBarrel::OnPlayerEnterSyncBarrel(int32 InPlayerNumber, FString InPlayerName, bool InIsLocalPlayer)
{
	if (PhotonReplicatorComponent->IsMine())
	{
		PhotonReplicatorComponent->SendReplicationData(true);
	}
}
