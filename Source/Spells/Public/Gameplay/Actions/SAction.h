// Spells - xixgames - juaxix - 2021/2022

#pragma once
#include "Gameplay/SActionsComponent.h"

class USActionsComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FSOnActionPostLoad, AActor*);

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
	USAction() : bIsActive(false) , bAutoStart(false) { }
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Spells|Actions")
	void StartAction(AActor* Instigator);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Spells|Actions")
	void StopAction(AActor* Instigator);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Spells|Animation")
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

	UFUNCTION(BlueprintPure, Category  = "Spells|Actions")
	bool IsAutoStart() const { return bAutoStart; }

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spells|UI")
	UTexture2D* Icon;
	
protected:
	UPROPERTY(VisibleInstanceOnly, Category = "Spells|Actions", meta = (AllowPrivateAccess = "true"))
	uint8 bIsActive:1;

	UPROPERTY(EditDefaultsOnly, Category = "Spells|Actions", meta = (AllowPrivateAccess = "true"))
	uint8 bAutoStart:1;
};
