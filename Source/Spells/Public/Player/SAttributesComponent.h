// Spells - xixgames - juaxix - 2021

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SAttributesComponent.generated.h"

 
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FSOnHealthAttributeChanged, AActor*, InstigatorActor, class USAttributesComponent*, AttributeComponent, float, NewHealth, float, Delta, const FHitResult&, Hit);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPELLS_API USAttributesComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	USAttributesComponent();

	UFUNCTION(BlueprintCallable, Category = "Spells|Attributes") FORCEINLINE
	bool ApplyHealthChange(float Delta, AActor* InstigatorActor, const FHitResult& Hit);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, BlueprintAssignable, Category = "Health")
	FSOnHealthAttributeChanged OnHealthAttributeChanged;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
	float Health = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
	float MaxHealth = 100.0f;
};
