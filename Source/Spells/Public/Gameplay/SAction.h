// Spells - xixgames - juaxix - 2021/2022

#pragma once
#include "SActionsComponent.h"

class USActionsComponent;

// Engine includes
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "SAction.generated.h"

/**
 * To create child classes as BPs for different actions used in the @see USActionComponent
 */
UCLASS(BlueprintType, Blueprintable)
class SPELLS_API USAction : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category = "Spells|Actions")
	void StartAction(AActor* Instigator);

	UFUNCTION(BlueprintNativeEvent, Category = "Spells|Actions")
	void StopAction(AActor* Instigator);

	UFUNCTION(BlueprintNativeEvent, Category = "Spells|Animation")
	void ReceiveAnimationNotif();
	virtual void ReceiveAnimationNotif_Implementation(){}

	UFUNCTION(BlueprintPure, Category = "Spells|Actions")
	USActionsComponent* GetOwningComponent() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Spells|Actions")
	bool CanStart(AActor* Instigator);
	virtual bool CanStart_Implementation(AActor* Instigator)
	{
		return !bIsActive && !GetOwningComponent()->ActiveGameplayTags.HasAny(BlockedTags);
	}

	UFUNCTION(BlueprintPure, Category  = "Spells|Actions")
	bool IsActive() const { return bIsActive; }

	virtual UWorld* GetWorld() const override
	{
		if (UActorComponent* ActorComponent = Cast<UActorComponent>(GetOuter()))
		{
			return ActorComponent->GetWorld();
		}

		return nullptr;
	}

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spells|Tags", meta = (Tooltip = "Tags added to the owning actor when activated, removed when the action stops"))
	FGameplayTagContainer GrantTags;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spells|Tags", meta = (Tooltip  = "Tag used to block the action if the owning actor has any of these tags applied"))
	FGameplayTagContainer BlockedTags;

	/* Action nickname to start/stop without a reference to the object */
	UPROPERTY(EditDefaultsOnly, Category = "Spells|Actions")
	FName ActionName;

	bool bIsActive = false;
};
