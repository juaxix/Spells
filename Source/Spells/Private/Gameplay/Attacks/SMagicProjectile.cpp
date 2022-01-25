// Spells - xixgames - juaxix - 2021/2022

#include "Gameplay/Attacks/SMagicProjectile.h"

// Unreal includes
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"

// Spells includes
#include "Gameplay/SGameplayBlueprintFunctions.h"
#include "Gameplay/SActionsComponent.h"
#include "Gameplay/Actions/SActionEffect.h"
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

	bReplicates = true;
	ProjectileMovementComponent->SetIsReplicated(true);

	InitialLifeSpan = 32.0f;
}

void ASMagicProjectile::OnSphereActorOverlap_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APawn* ThisInstigator = GetInstigator();
	if (IsValid(OtherActor) && OtherActor != ThisInstigator)
	{
		USActionsComponent* ActionsComponent = Cast<USActionsComponent>(OtherActor->GetComponentByClass(USActionsComponent::StaticClass()));
		if (IsValid(ActionsComponent) && ActionsComponent->ActiveGameplayTags.HasTag(CounterSpellTag))
		{
			ProjectileMovementComponent->Velocity = -ProjectileMovementComponent->Velocity/2.0f; // this also inverts the rotation
			SetInstigator(Cast<APawn>(OtherActor));
			SetActionEffectClasses(ActionsComponent->GetCounterSpellActionEffectClasses(), true);

			return;
		}

		if (USGameplayBlueprintFunctions::ApplyDamage(ThisInstigator, OtherActor, Damage, SweepResult))
		{
			OnProjectileStopped(SweepResult);
			if (ActionEffectClasses.Num() > 0 && IsValid(ActionsComponent) && HasAuthority())
			{
				for (TSubclassOf<USActionEffect> ActionEffect : ActionEffectClasses)
				{
					ActionsComponent->AddAction(ThisInstigator, ActionEffect);
				}
			}
		}
	}
}

void ASMagicProjectile::RemoveActionEffectClass(const TSubclassOf<USActionEffect>& ActionEffectClass)
{
	ActionEffectClasses.Remove(ActionEffectClass);
}

void ASMagicProjectile::SetActionEffectClasses(const TArray<TSubclassOf<USActionEffect>>& InActionEffectClasses, bool bAppend)
{
	if (bAppend)
	{
		ActionEffectClasses.Append(InActionEffectClasses);
	}
	else
	{
		ActionEffectClasses = InActionEffectClasses;
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
