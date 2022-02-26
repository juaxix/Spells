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

	UFUNCTION(BlueprintCallable, Category = "Spells|Actions")
	void SetActionsOwner(USActionsComponent* InActionsComponent, int32 InActionNetId);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Spells|Actions")
	void StartAction(AActor* Instigator);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Spells|Actions")
	void StopAction(AActor* Instigator);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Spells|Animation")
	void ReceiveAnimationNotif();
	virtual void ReceiveAnimationNotif_Implementation(){}

	UFUNCTION(BlueprintPure, Category = "Spells|Actions")
	USActionsComponent* GetOwningComponent() const{ return OwningActionsComponent; }

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

	UFUNCTION(BlueprintNativeEvent, Category  ="Spells|Photon Cloud")
	void OnPropertiesChanged(const UPhotonJSON* InActionJSON);

	// Generate a JSON with the minimum amount of data needed to build this action in a replicated USActionscomponent
	// using ActionClassName
	UFUNCTION(BlueprintNativeEvent, Category  ="Spells|Photon Cloud")
	UPhotonJSON* ToPhotonJSON() const;

	// Once the action is built in the replicated side, the only data needed for the action state change is generated to be synced
	// using the instigator
	UFUNCTION(BlueprintPure, Category = "Spells|Photon Cloud")
	UPhotonJSON* GetActionStateJSON(AActor* Instigator);

	// it will send this action state using GetActionStateJSON method using the owning component
	UFUNCTION(BlueprintCallable, Category  ="Spells|Photon Cloud")
	void ReplicateActionState(AActor* Instigator);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spells|Tags", meta = (Tooltip = "Tags added to the owning actor when activated, removed when the action stops"))
	FGameplayTagContainer GrantTags;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spells|Tags", meta = (Tooltip  = "Tag used to block the action if the owning actor has any of these tags applied"))
	FGameplayTagContainer BlockedTags;

	/* Action nickname to start/stop without a reference to the object */
	UPROPERTY(EditDefaultsOnly, Category = "Spells|Actions")
	FName ActionName;

	/* Action unique network name */
	UPROPERTY(EditDefaultsOnly, Category = "Spells|Actions")
	int32 ActionNetId = -1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spells|UI")
	TSoftObjectPtr<UTexture2D> Icon;
	
protected:
	UPROPERTY(VisibleInstanceOnly, Category = "Spells|Actions", meta = (AllowPrivateAccess = "true"))
	uint8 bIsActive:1;

	UPROPERTY(EditDefaultsOnly, Category = "Spells|Actions", meta = (AllowPrivateAccess = "true"))
	uint8 bAutoStart:1;

	UPROPERTY(VisibleInstanceOnly, Category = "Spells|Actions")
	USActionsComponent* OwningActionsComponent = nullptr;
};
