// Spells - xixgames - juaxix - 2021/2022

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SAttributesComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FSOnHealthAttributeChanged, AActor*, InstigatorActor, class USAttributesComponent*, AttributeComponent, float, NewHealth, float, Delta, const FHitResult&, Hit);

UENUM(BlueprintType)
enum class EAttributesTypes : uint8
{
	HEALTH = 0
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPELLS_API USAttributesComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	USAttributesComponent();

	UFUNCTION(BlueprintPure, Category = "Spells|Health") FORCEINLINE
	bool IsAlive() const
	{
		return Health > 0.0f;
	}
	
	UFUNCTION(BlueprintPure, Category = "Spells|Health") FORCEINLINE
	bool IsFullHealth() const { return Health == MaxHealth; }

	UFUNCTION(BlueprintCallable, Category = "Spells|Health")
	bool ApplyHealthChange(float Delta, AActor* InstigatorActor, const FHitResult& Hit);

	UFUNCTION(BlueprintPure, Category = "Spells|Health") FORCEINLINE
	float GetCurrentHealth() const { return Health; }

	UFUNCTION(BlueprintPure, Category = "Spells|Health") FORCEINLINE
	float GetMaximumHealth() const { return MaxHealth; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, BlueprintAssignable, Category = "Health")
	FSOnHealthAttributeChanged OnHealthAttributeChanged;
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
	float Health = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
	float MaxHealth = 100.0f;
};
