// Spells - xixgames - juaxix - 2021/2022

#include "Gameplay/SCharacterInteractionComponent.h"

// Unreal includes
#include "DrawDebugHelpers.h"
#include "GameFramework/Character.h"

// Spells includes
#include "Gameplay/SInteractableInterface.h"
#include "UI/SWorldUserWidget.h"

USCharacterInteractionComponent::USCharacterInteractionComponent()
	: bCheckForWalls(false)
#if !UE_BUILD_SHIPPING
	, bDebugInteraction(false)
#endif
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.01f;
	FCollisionObjectQueryParams CollisionObjectQueryParams;
	CollisionObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	CollisionObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
	CollisionObjectQueryParams_Static.AddObjectTypesToQuery(ECC_WorldStatic);
}

void USCharacterInteractionComponent::PrimaryAction()
{
	if (IsValid(TargetActor))
	{
		ISInteractableInterface::Execute_Interact(TargetActor, Cast<ACharacter>(GetOwner()));
	}
	else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Emerald, TEXT("Nothing to interact with"));
		}
	}
}

void USCharacterInteractionComponent::FindBestInteractable()
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
		AActor* NewTargetActor = nullptr;
		if (World->SweepMultiByObjectType(HitResults, StartLocation, EndLocation, FQuat::Identity, CollisionObjectQueryParams_StaticDynamic, CollisionShape, CollisionQueryParams))
		{
			for (FHitResult& HitResult: HitResults)
			{
				if (AActor* HitActor = HitResult.GetActor())
				{
					FHitResult WallHitResult;
					if (HitActor->Implements<USInteractableInterface>() && (!bCheckForWalls || 
						 World->LineTraceSingleByObjectType(WallHitResult, StartLocation, HitActor->GetActorLocation(), CollisionObjectQueryParams_Static) &&
						 (WallHitResult.GetActor() == HitActor || WallHitResult.GetActor() == nullptr)))
					{
						EndLocation = bCheckForWalls && WallHitResult.bBlockingHit ? WallHitResult.ImpactPoint : HitResult.ImpactPoint;
						NewTargetActor = HitActor;
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

		if (NewTargetActor != TargetActor)
		{
			OnFocusing(NewTargetActor);
		}

#if WITH_EDITORONLY_DATA
		if (bDebugInteraction)
		{
			DrawDebugCylinder(GetWorld(), StartLocation, 
				EndLocation, InteractionRadius, 12,  
				bBlockingHit || NewTargetActor? DebugColorActiveHit : DebugColorNoHit, 
					false, 0.01f, 0, 3);
		}
#endif
	}
}

void USCharacterInteractionComponent::OnFocusing(AActor* NewTargetActor)
{
	TargetActor = NewTargetActor;

	if (IsValid(TargetActor))
	{
		if (!IsValid(InteractionUserWidget) && IsValid(InteractionWidgetClass))
		{
			InteractionUserWidget = CreateWidget<USWorldUserWidget>(GetOwner()->GetInstigatorController<APlayerController>(),
									InteractionWidgetClass);
		}

		if (InteractionUserWidget && !InteractionUserWidget->IsInViewport())
		{
			InteractionUserWidget->AddToViewport();
		}
	}
	else
	{
		if (IsValid(InteractionUserWidget))
		{
			InteractionUserWidget->RemoveFromParent();
		}
	}

	if (InteractionUserWidget)
	{
		InteractionUserWidget->AttachedActor = TargetActor;
	}
}
