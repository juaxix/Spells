// Spells - xixgames - juaxix - 2021/2022

#pragma once

// Engine includes
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "Net/UnrealNetwork.h"

#include "SActionsComponent.generated.h"

class USActionsComponent;
class USActionEffect;
class USAction;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSOnActionStateChanged, USActionsComponent*, InOwningComponent, USAction*, InAction);

/**
 * Keeps a list of actions
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SPELLS_API USActionsComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USActionsComponent()
	{
		PrimaryComponentTick.bCanEverTick = true;
		SetIsReplicatedByDefault(true);
	}
	
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override
	{
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);

		DOREPLIFETIME(USActionsComponent, ActiveGameplayTags);
		DOREPLIFETIME(USActionsComponent, Actions);
	}

	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	
	UFUNCTION(BlueprintPure, Category = "Spells|Actions")
	bool HasAction(TSubclassOf<USAction> ActionClass) const;

	UFUNCTION(BlueprintPure, Category = "Spells|Actions")
	USAction* GetAction(TSubclassOf<USAction> ActionClass);

	UFUNCTION(BlueprintPure, Category = "Spells|Actions")
	const TArray<USAction*>& GetCurrentActions() const { return Actions; };

	UFUNCTION(BlueprintCallable, Category = "Spells|Actions")
	void AddAction(AActor* Instigator, TSubclassOf<USAction> ActionClass);

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Spells|Actions")
	void RemoveAction(USAction* Action);

	UFUNCTION(Server, Reliable)
	void Server_StartActionByName(AActor* Instigator, const FName& ActionName);

	UFUNCTION(BlueprintCallable, Category = "Spells|Actions")
	bool StartActionByName(AActor* Instigator, const FName& ActionName);

	UFUNCTION(Server, Reliable)
	void Server_StopActionByName(AActor* Instigator, const FName& ActionName);

	UFUNCTION(BlueprintCallable, Category = "Spells|Actions")
	bool StopActionByName(AActor* Instigator, const FName& ActionName);

	UFUNCTION(BlueprintCallable, Category = "Spells|Animation")
	virtual void ReceiveAnimNotif(AActor* Instigator, const FName& ActionName);

	UFUNCTION(BlueprintPure, Category = "Spells|Action Effects")
	virtual const TArray<TSubclassOf<USActionEffect>>& GetCounterSpellActionEffectClasses() const { return CounterSpellActionEffectClasses; }

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Spells|Tags")
	FGameplayTagContainer ActiveGameplayTags;

	UPROPERTY(BlueprintAssignable, BlueprintReadOnly, Category = "Spells|Actions")
	FSOnActionStateChanged OnActionStarted;

	UPROPERTY(BlueprintAssignable, BlueprintReadOnly, Category = "Spells|Actions")
	FSOnActionStateChanged OnActionStopped;
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spells|Actions", meta = (AllowPrivateAccess = true, Tooltip = "Actions classes to be instanced on start"))
	TArray<TSubclassOf<USAction>> DefaultActions;

	UPROPERTY(Replicated, VisibleInstanceOnly, BlueprintReadOnly, Category = "Spells|Actions", meta = (AllowPrivateAccess = true))
	TArray<USAction*> Actions;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spells|Action Effects", meta = (AllowPrivateAccess = true, Tooltip = "Actions Effect classes to be append on counter attack"))
	TArray<TSubclassOf<USActionEffect>> CounterSpellActionEffectClasses;
};
