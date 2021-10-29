// Spells - xixgames - juaxix - 2021

#include "Attacks/SMagicProjectile.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"

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

void ASMagicProjectile::BeginPlay()
{
	Super::BeginPlay();
	SphereComponent->IgnoreActorWhenMoving(GetInstigator(), true);
}
