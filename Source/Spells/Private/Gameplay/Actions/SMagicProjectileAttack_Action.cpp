// Spells - xixgames - juaxix - 2021/2022

#include "Gameplay/Actions/SMagicProjectileAttack_Action.h"

// Unreal includes
#include "DrawDebugHelpers.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/Character.h"

// Spells includes
#include "PhotonJSON.h"
#include "Online/SPhotonCloudObject.h"
#include "Gameplay/Attacks/SMagicProjectile.h"
#include "Player/SCharacter.h"

namespace
{
	constexpr float MaxHitScanDistanceLook = 10000.0f;
}

void USMagicProjectileAttack_Action::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);
	Character = Cast<ACharacter>(Instigator);
	if (IsValid(Character))
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
	
	Owner_DoMagicalAttack();

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

void USMagicProjectileAttack_Action::OnPropertiesChanged_Implementation(const UPhotonJSON* InActionJSON)
{
	Super::OnPropertiesChanged_Implementation(InActionJSON);
	if (InActionJSON && InActionJSON->Contains(SpellsKeysForReplication::SpawnProjectile))
	{
		const UPhotonJSON* ProjectileJSON = InActionJSON->Get_JSON_Object(SpellsKeysForReplication::SpawnProjectile);
		SpawnMagicProjectile(
			ProjectileJSON->GetVector(SpellsKeysForReplication::ProjectileLocation),
			ProjectileJSON->GetRotator(SpellsKeysForReplication::ProjectileRotation)
		);
	}
}

void USMagicProjectileAttack_Action::Owner_DoMagicalAttack()
{
	ensureAlwaysMsgf(AttackProjectileClass != nullptr, TEXT("AttackProjectileClass needs a class"));
	if (!GetOwningComponent()->HasPhotonAuthority())
	{
		return;
	}

	FRotator ProjectileRotation;
	UWorld* World = GetWorld();
	FVector HandLocation = SkeletalSocket->GetSocketLocation(Character->GetMesh());

	// Local player
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

	SpawnMagicProjectile(SkeletalSocket->GetSocketLocation(Character->GetMesh()), ProjectileRotation);
}

void USMagicProjectileAttack_Action::SpawnMagicProjectile(const FVector& Location, const FRotator& Rotation)
{
	const FTransform SpawnTransform = FTransform(Rotation, Location);
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	SpawnParams.Instigator = Character;

	GetWorld()->SpawnActor<ASMagicProjectile>(AttackProjectileClass, SpawnTransform, SpawnParams);

	if (GetOwningComponent()->HasPhotonAuthority())
	{
		GetOwningComponent()->ReplicateActionEvent(
		UPhotonJSON::Create(this)->Set_JSON_Object(FString::FromInt(ActionNetId), 
			UPhotonJSON::Create(this)->Set_JSON_Object(SpellsKeysForReplication::SpawnProjectile, 
				UPhotonJSON::Create(this)
					->SetRotator(SpellsKeysForReplication::ProjectileRotation, Rotation)
					->SetVector(SpellsKeysForReplication::ProjectileLocation, Location)
			)));
	}
}
