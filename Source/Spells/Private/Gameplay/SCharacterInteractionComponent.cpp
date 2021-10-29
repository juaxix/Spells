// Spells - xixgames - juaxix - 2021

#include "Gameplay/SCharacterInteractionComponent.h"

#include "DrawDebugHelpers.h"
#include "GameFramework/Character.h"
#include "Gameplay/SInteractableInterface.h"


USCharacterInteractionComponent::USCharacterInteractionComponent()
	: bDebugInteraction(false)
	, bCheckForWalls(false)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USCharacterInteractionComponent::PrimaryAction()
{
	if (ACharacter* MyOwner = Cast<ACharacter>(GetOwner()))
	{
		FHitResult HitResult;
		FVector StartLocation = MyOwner->GetActorLocation() + InteractionOffset;
		FVector EndLocation = StartLocation + MyOwner->GetControlRotation().Vector() * MaxInteractionDistance;
		FCollisionObjectQueryParams CollisionObjectQueryParams;
		CollisionObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
		FCollisionQueryParams CollisionQueryParams;
		CollisionQueryParams.AddIgnoredActor(MyOwner);
		FCollisionShape CollisionShape;
		CollisionShape.SetSphere(InteractionRadius);
		UWorld* World = GetWorld();

		if (World->SweepSingleByObjectType(HitResult, StartLocation, EndLocation, FQuat::Identity, CollisionObjectQueryParams, CollisionShape, CollisionQueryParams))
		{
			if (AActor* HitActor = HitResult.GetActor())
			{
				FHitResult WallHitResult;
				CollisionObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
				if (HitActor->Implements<USInteractableInterface>() &&  (!bCheckForWalls || 
					 World->LineTraceSingleByObjectType(WallHitResult, StartLocation, HitResult.ImpactPoint, CollisionObjectQueryParams) &&
					 WallHitResult.GetActor() == HitActor))
				{
					ISInteractableInterface::Execute_Interact(HitActor, MyOwner);
					EndLocation = bCheckForWalls && WallHitResult.bBlockingHit ? WallHitResult.ImpactPoint : HitResult.ImpactPoint;
				}
				else
				{
					EndLocation = HitResult.ImpactPoint;
				}
			}
			else
			{
				EndLocation = HitResult.ImpactPoint;
			}
		}

		if (bDebugInteraction)
		{
			DrawDebugCylinder(GetWorld(), StartLocation, 
				EndLocation, InteractionRadius, 12,  
				HitResult.bBlockingHit ? FColor::Green : FColor::Silver, 
					false, 0.5f, 0, 2);
		}


	}
}
