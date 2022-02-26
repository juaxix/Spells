// Spells - xixgames - juaxix - 2021/2022

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UI/SWorldUserWidget.h"
#include "SCharacterInteractionComponent.generated.h"

class USWorldUserWidget;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPELLS_API USCharacterInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USCharacterInteractionComponent();

	virtual void PrimaryAction();

protected:
	virtual void FindBestInteractable();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override
	{
		Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
		FindBestInteractable();
	}

	virtual void OnFocusing(AActor* NewTargetActor);

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spells|Setup")
	float MaxInteractionDistance = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spells|Setup")
	FVector InteractionOffset = FVector(0.0f, 0.0f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spells|Setup")
	float InteractionRadius = 30.0f;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spells|Setup", meta = (AllowPrivateAccess = "true"))
	uint8 bCheckForWalls:1;

	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spells|Setup", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<USWorldUserWidget> InteractionWidgetClass;

	UPROPERTY(Transient)
	USWorldUserWidget* InteractionUserWidget = nullptr;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Spells|Interaction", meta = (AllowPrivateAcces = "true"))
	AActor* TargetActor = nullptr;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spells|Setup", meta = (AllowPrivateAccess = "true"))
	uint8 bDebugInteraction:1;

	FColor DebugColorActiveHit = FColor(0,255,0, 128);
	FColor DebugColorNoHit = FColor(189, 195, 199, 128);
#endif

	FCollisionObjectQueryParams CollisionObjectQueryParams_StaticDynamic, CollisionObjectQueryParams_Static;
};
