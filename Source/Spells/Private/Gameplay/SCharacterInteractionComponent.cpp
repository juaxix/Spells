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
	FCollisionObjectQueryParams CollisionObjectQueryParams;
	CollisionObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	CollisionObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
	CollisionObjectQueryParams_Static.AddObjectTypesToQuery(ECC_WorldStatic);
}

void USCharacterInteractionComponent::PrimaryAction()
{
	if (ACharacter* MyOwner = Cast<ACharacter>(GetOwner()))
	{
		TArray<FHitResult> HitResults;
		bool bBlockingHit = false;
		FVector StartLocation = MyOwner->GetActorLocation() + InteractionOffset + FVector::UpVector * MyOwner->BaseEyeHeight;
		FVector EndLocation = StartLocation + MyOwner->GetControlRotation().Vector() * MaxInteractionDistance;
		FCollisionQueryParams CollisionQueryParams;
		CollisionQueryParams.AddIgnoredActor(MyOwner);
		FCollisionShape CollisionShape;
		CollisionShape.SetSphere(InteractionRadius);
		UWorld* World = GetWorld();
		if (World->SweepMultiByObjectType(HitResults, StartLocation, EndLocation, FQuat::Identity, CollisionObjectQueryParams_StaticDynamic, CollisionShape, CollisionQueryParams))
		{
			for (auto& HitResult: HitResults)
			{
				if (AActor* HitActor = HitResult.GetActor())
				{
					FHitResult WallHitResult;
					if (HitActor->Implements<USInteractableInterface>() && (!bCheckForWalls || 
						 World->LineTraceSingleByObjectType(WallHitResult, StartLocation, HitActor->GetActorLocation(), CollisionObjectQueryParams_Static) &&
						 (WallHitResult.GetActor() == HitActor || WallHitResult.GetActor() == nullptr)))
					{
						ISInteractableInterface::Execute_Interact(HitActor, MyOwner);
						EndLocation = bCheckForWalls && WallHitResult.bBlockingHit ? WallHitResult.ImpactPoint : HitResult.ImpactPoint;
						break;
					}
					
					EndLocation = HitResult.ImpactPoint;
					bBlockingHit = HitResult.bBlockingHit;
				}
				else
				{
					EndLocation = HitResult.ImpactPoint;
				}
			}
		}

		if (bDebugInteraction)
		{
			DrawDebugCylinder(GetWorld(), StartLocation, 
				EndLocation, InteractionRadius, 12,  
				bBlockingHit ? FColor::Green : FColor::Silver, 
					false, 0.5f, 0, 2);
		}
	}
}
