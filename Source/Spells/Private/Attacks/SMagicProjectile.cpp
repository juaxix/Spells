// Spells - xixgames - juaxix - 2021/2022

#include "Attacks/SMagicProjectile.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Player/SAttributesComponent.h"
#include "Player/SCharacter.h"

ASMagicProjectile::ASMagicProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	EffectComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("EffectComponent"));
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));

	EffectComponent->SetupAttachment(SphereComponent);

	SphereComponent->SetCollisionObjectType(ECC_WorldDynamic);
	SphereComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	SphereComponent->SetCollisionProfileName(SpellsGame::MagicProjectileChannel);
	SphereComponent->CanCharacterStepUpOn = ECB_No;
	
	ProjectileMovementComponent->InitialSpeed = 1000.0f;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->bInitialVelocityInLocalSpace = true;
}

void ASMagicProjectile::OnSphereActorHit_Implementation(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	OnSphereActorOverlap(HitComponent, OtherActor, OtherComp, 0, false, Hit);
}

void ASMagicProjectile::OnSphereActorOverlap_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APawn* ThisInstigator = GetInstigator();
	if (IsValid(OtherActor) && IsValid(ThisInstigator) && OtherActor != ThisInstigator )
	{
		if (USAttributesComponent* Attributes = Cast<USAttributesComponent>(OtherActor->GetComponentByClass(USAttributesComponent::StaticClass())))
		{
			Attributes->ApplyHealthChange(-Damage, ThisInstigator, SweepResult);
			OnProjectileStopped(SweepResult);
		}
	}
}

void ASMagicProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (!HasAnyFlags(RF_ClassDefaultObject|RF_DefaultSubObject|RF_Transient))
	{
		SphereComponent->IgnoreActorWhenMoving(GetInstigator(), true);
		SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ASMagicProjectile::OnSphereActorOverlap);
		SphereComponent->OnComponentHit.AddDynamic(this, &ASMagicProjectile::OnSphereActorHit);
		ProjectileMovementComponent->OnProjectileStop.AddDynamic(this, &ASMagicProjectile::OnProjectileStopped);
	}
}
