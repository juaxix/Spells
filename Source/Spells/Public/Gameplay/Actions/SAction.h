// Spells - xixgames - juaxix - 2021/2022

#pragma once
#include "Gameplay/SActionsComponent.h"

class USActionsComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FSOnActionPostLoad, AActor*);

// Engine includes
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Net/UnrealNetwork.h"

#include "SAction.generated.h"

USTRUCT(BlueprintType)
struct FSActionRepData
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category = "Spells|Actions|Replication")
	bool bIsRunning = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category = "Spells|Actions|Replication")
	AActor* Instigator = nullptr;
};

/**
 * To create child classes as BPs for different actions used in the @see USActionComponent
 */
UCLASS(BlueprintType, Blueprintable)
class SPELLS_API USAction : public UObject
{
	GENERATED_BODY()

public:
	USAction() : bAutoStart(false) { }

	UFUNCTION(BlueprintCallable, Category = "Spells|Actions")
	void SetActionsOwner(USActionsComponent* InActionsComponent) { OwningActionsComponent = InActionsComponent; }

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Spells|Actions")
	void StartAction(AActor* Instigator);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Spells|Actions")
	void StopAction(AActor* Instigator);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Spells|Animation")
	void ReceiveAnimationNotif();
	virtual void ReceiveAnimationNotif_Implementation(){}

	UFUNCTION(BlueprintPure, Category = "Spells|Actions")
	USActionsComponent* GetOwningComponent() const { return OwningActionsComponent; }

	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Spells|Actions")
	bool CanStart(AActor* Instigator);
	virtual bool CanStart_Implementation(AActor* Instigator)
	{
		return !ActionRepData.bIsRunning && !GetOwningComponent()->ActiveGameplayTags.HasAny(BlockedTags);
	}

	UFUNCTION(BlueprintPure, Category  = "Spells|Actions")
	bool IsActive() const { return ActionRepData.bIsRunning; }

	UFUNCTION(BlueprintPure, Category  = "Spells|Actions")
	bool IsAutoStart() const { return bAutoStart; }

	UFUNCTION()
	void OpRep_IsActive()
	{
		if (ActionRepData.bIsRunning)
		{
			StartAction(ActionRepData.Instigator);
		}
		else
		{
			StopAction(ActionRepData.Instigator);
		}
	}

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override
	{
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);

		DOREPLIFETIME(USAction, ActionRepData);
		DOREPLIFETIME(USAction, OwningActionsComponent);
	}

	bool IsSupportedForNetworking() const override { return true; }

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spells|Tags", meta = (Tooltip = "Tags added to the owning actor when activated, removed when the action stops"))
	FGameplayTagContainer GrantTags;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spells|Tags", meta = (Tooltip  = "Tag used to block the action if the owning actor has any of these tags applied"))
	FGameplayTagContainer BlockedTags;

	/* Action nickname to start/stop without a reference to the object */
	UPROPERTY(EditDefaultsOnly, Category = "Spells|Actions")
	FName ActionName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spells|UI")
	TSoftObjectPtr<UTexture2D> Icon;
	
protected:
	UPROPERTY(Replicated, VisibleInstanceOnly, Category = "Spells|Actions")
	USActionsComponent* OwningActionsComponent = nullptr;

	UPROPERTY(ReplicatedUsing = OpRep_IsActive, VisibleInstanceOnly, Category = "Spells|Actions", meta = (AllowPrivateAccess = "true"))
	FSActionRepData ActionRepData;

	UPROPERTY(EditDefaultsOnly, Category = "Spells|Actions", meta = (AllowPrivateAccess = "true"))
	uint8 bAutoStart:1;
};
