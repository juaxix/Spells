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

void USMagicProjectileAttack_Action::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);
	Character = Cast<ACharacter>(Instigator);
	if (ensureAlwaysMsgf(IsValid(Character), TEXT("Character for a Magic Projectile Attack action must be valid")))
	{
		SkeletalSocket = Character->GetMesh()->GetSocketByName(SkeletalSocketName);
		check(SkeletalSocket);

		if (bIsRepeatedAttack)
		{
			bRepeatingAction = true;
		}

		Character->PlayAnimMontage(AnimMontage);
	}
}

void USMagicProjectileAttack_Action::ReceiveAnimationNotif_Implementation()
{
	Super::ReceiveAnimationNotif_Implementation();
	DoMagicalAttack();
	if (bIsActive)
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

bool USMagicProjectileAttack_Action::DoMagicalAttack()
{
	ensureAlwaysMsgf(AttackProjectileClass != nullptr, TEXT("AttackProjectileClass needs a class"));
	FRotator ProjectileRotation;
	UWorld* World = GetWorld();
	FVector HandLocation = SkeletalSocket->GetSocketLocation(Character->GetMesh());
	
	if (APlayerController* PlayerController = Character->GetController<APlayerController>())
	{
		ProjectileRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
		FHitResult Hit;
		FCollisionQueryParams CollisionQueryParams;
		CollisionQueryParams.AddIgnoredActor(Character);
		FVector LookStart = PlayerController->PlayerCameraManager->GetCameraLocation();
		FVector LookEnd = LookStart + PlayerController->PlayerCameraManager->GetActorForwardVector() * MaxHitScanDistanceLook;
		
		if (World->LineTraceSingleByObjectType(Hit, LookStart, LookEnd, FCollisionObjectQueryParams::AllObjects, CollisionQueryParams))
		{
			LookEnd = Hit.ImpactPoint;
			if (bDebugMode)
			{
				DrawDebugSphere(World, LookEnd, 23.f, 12, FColor::Red, false, 1.5f);
			}

			ProjectileRotation = FRotationMatrix::MakeFromX(LookEnd - HandLocation).Rotator();
		}
	}
	else
	{
		ProjectileRotation = Character->GetControlRotation();
	}

	const FTransform SpawnTransform = FTransform(ProjectileRotation, HandLocation);
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	SpawnParams.Instigator = Character;

	return IsValid(World->SpawnActor<ASMagicProjectile>(AttackProjectileClass, SpawnTransform, SpawnParams));
}
