// Spells - xixgames - juaxix - 2021/2022

#pragma once

// Engine includes
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "SActionsComponent.generated.h"

class USAction;

/**
 * Keep a list of actions
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPELLS_API USActionsComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USActionsComponent();
	
	UFUNCTION(BlueprintCallable, Category = "Spells|Actions")
	void AddAction(TSubclassOf<USAction> ActionClass);

	UFUNCTION(BlueprintCallable, Category = "Spells|Actions")
	bool StartActionByName(AActor* Instigator, const FName& ActionName);

	UFUNCTION(BlueprintCallable, Category = "Spells|Actions")
	bool StopActionByName(AActor* Instigator, const FName& ActionName);

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Spells|Actions", meta = (AllowPrivateAccess = true))
	TArray<USAction*> Actions;
};
