// Spells - xixgames - juaxix - 2021/2022

#pragma once

// Engine includes
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Character.h"

#include "SActionsComponent.generated.h"

class USActionEffect;
class USAction;

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
	}
	
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION(BlueprintPure, Category = "Spells|Actions")
	bool HasAction(TSubclassOf<USAction> ActionClass) const;

	UFUNCTION(BlueprintPure, Category = "Spells|Actions")
	USAction* GetAction(TSubclassOf<USAction> ActionClass);

	UFUNCTION(BlueprintCallable, Category = "Spells|Actions")
	void AddAction(AActor* Instigator, TSubclassOf<USAction> ActionClass);

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Spells|Actions")
	void RemoveAction(USAction* Action);

	UFUNCTION(BlueprintCallable, Category = "Spells|Actions")
	bool StartActionByName(AActor* Instigator, const FName& ActionName);

	UFUNCTION(BlueprintCallable, Category = "Spells|Actions")
	bool StopActionByName(AActor* Instigator, const FName& ActionName);

	UFUNCTION(BlueprintCallable, Category = "Spells|Animation")
	virtual void ReceiveAnimNotif(AActor* Instigator, const FName& ActionName);

	UFUNCTION(BlueprintPure, Category = "Spells|Action Effects")
	virtual const TArray<TSubclassOf<USActionEffect>>& GetCounterSpellActionEffectClasses() const { return CounterSpellActionEffectClasses; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spells|Tags")
	FGameplayTagContainer ActiveGameplayTags;
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spells|Actions", meta = (AllowPrivateAccess = true, Tooltip = "Actions classes to be instanced on start"))
	TArray<TSubclassOf<USAction>> DefaultActions;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Spells|Actions", meta = (AllowPrivateAccess = true))
	TArray<USAction*> Actions;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spells|Action Effects", meta = (AllowPrivateAccess = true, Tooltip = "Actions Effect classes to be append on counter attack"))
	TArray<TSubclassOf<USActionEffect>> CounterSpellActionEffectClasses;
};
