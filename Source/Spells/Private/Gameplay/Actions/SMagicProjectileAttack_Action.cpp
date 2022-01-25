// Spells - xixgames - juaxix - 2021/2022

#include "Gameplay/Actions/SMagicProjectileAttack_Action.h"

// Unreal includes
#include "DrawDebugHelpers.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/Character.h"

// Spells includes
#include "Gameplay/Attacks/SMagicProjectile.h"

namespace
{
	constexpr float MaxHitScanDistanceLook = 10000.0f;
}

FCollisionObjectQueryParams USMagicProjectileAttack_Action::ObjParams;

USMagicProjectileAttack_Action::USMagicProjectileAttack_Action()
	: bIsRepeatedAttack(false)
	, bRepeatingAction(false)
{
	if (!ObjParams.IsValid())
	{
		ObjParams.AddObjectTypesToQuery(ECC_WorldDynamic);
		ObjParams.AddObjectTypesToQuery(ECC_WorldStatic);
		ObjParams.AddObjectTypesToQuery(ECC_Pawn);
	}
}

void USMagicProjectileAttack_Action::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);
	if (Instigator)
	{
		Character = Cast<ACharacter>(Instigator);
	}

	if (bIsRepeatedAttack)
	{
		bRepeatingAction = true;
	}

	//if (ensureAlwaysMsgf(IsValid(Character), TEXT("Character for a Magic Projectile Attack action must be valid")))
	{
		if (Character)
		{
			SkeletalSocket = Character->GetMesh()->GetSocketByName(SkeletalSocketName);
			Character->PlayAnimMontage(AnimMontage);
		}
	}
}

void USMagicProjectileAttack_Action::ReceiveAnimationNotif_Implementation()
{
	Super::ReceiveAnimationNotif_Implementation();
	if (IsValid(Character) && Character->HasAuthority())
	{
		DoMagicalAttack();
	}

	if (ActionRepData.bIsRunning)
	{
		if (bRepeatingAction)
		{
			Character->PlayAnimMontage(AnimMontage);
		}
		else
		{
			StopAction_Implementation(Character);
		}
	}
}

void USMagicProjectileAttack_Action::DoMagicalAttack()
{
	ensureAlwaysMsgf(AttackProjectileClass != nullptr, TEXT("AttackProjectileClass needs a class"));

	FRotator ProjectileRotation;
	UWorld* World = GetWorld();
	FVector HandLocation = SkeletalSocket->GetSocketLocation(Character->GetMesh());
	
	ProjectileRotation = Character->GetControlRotation();
	FHitResult Hit;
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(Character);
	FVector LookStart = Character->GetActorLocation() + FVector(0.0f,0.0f,Character->BaseEyeHeight);
	FVector LookEnd = LookStart + ProjectileRotation.Vector() * MaxHitScanDistanceLook;
	
	if (World->LineTraceSingleByObjectType(Hit, LookStart, LookEnd, ObjParams, CollisionQueryParams))
	{
		LookEnd = Hit.ImpactPoint;
		if (bDebugMode)
		{
			DrawDebugSphere(World, LookEnd, 23.f, 12, FColor::Red, false, 1.5f);
		}

		ProjectileRotation = FRotationMatrix::MakeFromX(LookEnd - HandLocation).Rotator();
	}
	
	const FTransform SpawnTransform = FTransform(ProjectileRotation, HandLocation);
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	SpawnParams.Instigator = Character;
	if (!IsValid(World->SpawnActor<ASMagicProjectile>(AttackProjectileClass, SpawnTransform, SpawnParams)))
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not spawn a magic projectile %s"), *GetNameSafe(AttackProjectileClass));
	}
}
