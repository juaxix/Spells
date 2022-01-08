// Spells - xixgames - juaxix - 2021/2022

#pragma once

// Engine includes
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
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
	USActionsComponent()
	{
		PrimaryComponentTick.bCanEverTick = true;
	}
	
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override
	{
		Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
		if (GEngine)
		{
			FString DebugMsg = GetNameSafe(GetOwner()) + " : " + ActiveGameplayTags.ToStringSimple();
			GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Green, DebugMsg);
		}
	}
	
	UFUNCTION(BlueprintCallable, Category = "Spells|Actions")
	void AddAction(TSubclassOf<USAction> ActionClass);

	UFUNCTION(BlueprintCallable, Category = "Spells|Actions")
	bool StartActionByName(AActor* Instigator, const FName& ActionName);

	UFUNCTION(BlueprintCallable, Category = "Spells|Actions")
	bool StopActionByName(AActor* Instigator, const FName& ActionName);

	UFUNCTION(BlueprintCallable, Category = "Spells|Animation")
	virtual void ReceiveAnimNotif(AActor* Instigator, const FName& ActionName);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spells|Tags")
	FGameplayTagContainer ActiveGameplayTags;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spells|Actions", meta = (AllowPrivateAccess = true, Tooltip = "Actions classes to be instanced on start"))
	TArray<TSubclassOf<USAction>> DefaultActions;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Spells|Actions", meta = (AllowPrivateAccess = true))
	TArray<USAction*> Actions;
};
