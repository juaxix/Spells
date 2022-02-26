// Spells - xixgames - juaxix - 2021/2022

#pragma once

// Engine includes
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"

// Spells includes
#include "Online/SPhotonCloudObject.h"

#include "SActionsComponent.generated.h"

class UPhotonJSON;
class USPhotonCloudObject;
class USActionsComponent;
class USActionEffect;
class USAction;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSOnActionStateChanged, USActionsComponent*, InOwningComponent, USAction*, InAction);

/**
 * Keep a list of actions
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPELLS_API USActionsComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USActionsComponent()
	{
		PrimaryComponentTick.bCanEverTick = true;
		Actions.Empty(DefaultActions.Num());
	}

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void ReplicateActionEvent(UPhotonJSON* ActionEventJSON);

	// replicate the current list of actions (only ActionUniqueNetId and ClassName needed)
	virtual void SyncActions();

	UFUNCTION(BlueprintPure, Category = "Spells|Photon Cloud")
	bool HasPhotonAuthority() const;

	UFUNCTION(BlueprintNativeEvent, Category = "Spells|Photon Cloud")
	void OnActionsPropertiesChanged(UPhotonJSON* ActionsJSON);

	UFUNCTION(BlueprintPure, Category = "Spells|Actions")
	bool HasAction(TSubclassOf<USAction> ActionClass) const;

	UFUNCTION(BlueprintPure, Category = "Spells|Actions")
	USAction* GetAction(TSubclassOf<USAction> ActionClass);

	UFUNCTION(BlueprintPure, Category = "Spells|Actions")
	const TArray<USAction*>& GetCurrentActions() const { return Actions; }

	UFUNCTION(BlueprintCallable, Category = "Spells|Actions")
	USAction* AddAction(AActor* Instigator, TSubclassOf<USAction> ActionClass);

	UFUNCTION(BlueprintCallable, Category = "Spells|Actions")
	void CreateDefaultActions();

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Spells|Actions")
	void RemoveAction(USAction* Action);

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Spells|Actions")
	void RemoveAllActions()
	{
		Actions.Empty();
	}

	UFUNCTION(BlueprintCallable, Category = "Spells|Actions")
	bool StartActionByName(AActor* Instigator, const FName& ActionName);

	UFUNCTION(BlueprintCallable, Category = "Spells|Actions")
	bool StopActionByName(AActor* Instigator, const FName& ActionName);

	UFUNCTION(BlueprintCallable, Category = "Spells|Animation")
	virtual void ReceiveAnimNotif(AActor* Instigator, const FName& ActionName);

	UFUNCTION(BlueprintPure, Category = "Spells|Action Effects")
	virtual const TArray<TSubclassOf<USActionEffect>>& GetCounterSpellActionEffectClasses() const { return CounterSpellActionEffectClasses; }

	void SetupPhoton(int32 ActorUniqueId, ESInstigatorTypes InstigatorType);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spells|Tags")
	FGameplayTagContainer ActiveGameplayTags;

	UPROPERTY(BlueprintAssignable, BlueprintReadOnly, Category = "Spells|Actions")
	FSOnActionStateChanged OnActionStarted;

	UPROPERTY(BlueprintAssignable, BlueprintReadOnly, Category = "Spells|Actions")
	FSOnActionStateChanged OnActionStopped;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Spells|Photon Cloud")
	USPhotonCloudObject* PhotonCloudObject = nullptr;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Spells|Photon Cloud", meta = (AllowPrivateAccess = true, Tooltip = "Player number or actor unique id of the owner of this component"))
	int32 OwnerUniqueNetId = -1;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Spells|Photon Cloud", meta = (AllowPrivateAccess = true, Tooltip = "Player number or actor unique id of the owner of this component"))
	ESInstigatorTypes OwnerInstigatorType = ESInstigatorTypes::INVALID;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spells|Actions", meta = (AllowPrivateAccess = true, Tooltip = "Actions classes to be instanced on start"))
	TArray<TSubclassOf<USAction>> DefaultActions;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Spells|Actions", meta = (AllowPrivateAccess = true))
	TArray<USAction*> Actions = TArray<USAction*>();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spells|Action Effects", meta = (AllowPrivateAccess = true, Tooltip = "Actions Effect classes to be append on counter attack"))
	TArray<TSubclassOf<USActionEffect>> CounterSpellActionEffectClasses;

	int32 ActionNetIdCounter = -1;
};
