// Spells - xixgames - juaxix - 2021

#include "Gameplay/SCharacterInteractionComponent.h"

#include "DrawDebugHelpers.h"
#include "GameFramework/Character.h"
#include "Gameplay/SInteractableInterface.h"


USCharacterInteractionComponent::USCharacterInteractionComponent()
	: bDebugInteraction(false)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USCharacterInteractionComponent::PrimaryAction()
{
	if (ACharacter* MyOwner = Cast<ACharacter>(GetOwner()))
	{
		FHitResult HitResult;
		FVector StartLocation = MyOwner->GetActorLocation() + InteractionOffset;
		FVector EndLocation = StartLocation + (MyOwner->GetControlRotation().Vector() * MaxInteractionDistance);
		FCollisionObjectQueryParams CollisionObjectQueryParams;
		CollisionObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
		FCollisionQueryParams CollisionQueryParams;
		CollisionQueryParams.AddIgnoredActor(MyOwner);
		FCollisionShape CollisionShape;
		CollisionShape.SetSphere(InteractionRadius);

		if (GetWorld()->SweepSingleByObjectType(HitResult, StartLocation, EndLocation, FQuat::Identity, CollisionObjectQueryParams, CollisionShape, CollisionQueryParams))
		{
			EndLocation = HitResult.ImpactPoint;
			if (AActor* HitActor = HitResult.GetActor())
			{
				if (HitActor->Implements<USInteractableInterface>())
				{
					ISInteractableInterface::Execute_Interact(HitActor, MyOwner);
				}
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
