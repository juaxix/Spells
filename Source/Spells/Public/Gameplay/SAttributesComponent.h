// Spells - xixgames - juaxix - 2021/2022

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Online/SPhotonCloudObject.h"
#include "SAttributesComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FSOnAttributeChanged, AActor*, InstigatorActor,
											  class USAttributesComponent*, AttributeComponent, float, NewValue, float,
											  Delta, const FHitResult&, Hit);

UENUM(BlueprintType)
enum class EAttributesTypes : uint8
{
	HEALTH = 0,
	MANA,
	RAGE
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPELLS_API USAttributesComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	USAttributesComponent();

	void SetupPhoton(int32 ActorUniqueId, ESInstigatorTypes ActorInstigatorType);

	UFUNCTION(BlueprintPure, Category = "Spells|Attributes")
	static USAttributesComponent* GetAttributesComponent(const AActor* FromActor)
	{
		return IsValid(FromActor) ? Cast<USAttributesComponent>(FromActor->GetComponentByClass(USAttributesComponent::StaticClass())) : nullptr;
	}

	UFUNCTION(BlueprintPure, Category = "Spells|Health") FORCEINLINE
	bool IsAlive() const { return Health > 0.0f;}
	
	UFUNCTION(BlueprintPure, Category = "Spells|Health") FORCEINLINE
	bool IsFullHealth() const { return Health == MaxHealth; }

	UFUNCTION(BlueprintPure, Category = "Spells|Photon Cloud") FORCEINLINE
	bool HasPhotonAuthory() const
	{
		return (OwnerInstigatorType == ESInstigatorTypes::PLAYER && PhotonCloudObject->GetPlayerNumber() == OwnerUniqueNetId)
									|| 
			   (OwnerInstigatorType == ESInstigatorTypes::MASTER_AI && PhotonCloudObject->AmIMaster());
	}

	UFUNCTION(BlueprintNativeEvent, Category = "Spells|Photon Cloud")
	void OnAttributesPropertiesChanged(UPhotonJSON* AttributesJSON);
	
	UFUNCTION(BlueprintCallable, Category = "Spells|Health")
	bool ApplyHealthChange(float Delta, AActor* InstigatorActor, const FHitResult& Hit);

	UFUNCTION(BlueprintPure, Category = "Spells|Health") FORCEINLINE
	float GetCurrentHealth() const { return Health; }

	UFUNCTION(BlueprintPure, Category = "Spells|Health") FORCEINLINE
	float GetMaximumHealth() const { return MaxHealth; }

	UFUNCTION(Exec, BlueprintCallable, BlueprintPure = false, Category = "Spells|Health")
	void Kill(AActor* KillerActor = nullptr) { ApplyHealthChange(-MaxHealth, KillerActor, FHitResult()); }

	UFUNCTION(BlueprintCallable, Category = "Spells|Mana")
	void ApplyManaChange(float Delta, AActor* InstigatorActor, const FHitResult& Hit);

	UFUNCTION(BlueprintPure, Category = "Spells|Mana") FORCEINLINE
	float GetCurrentMana() const { return Mana; }

	UFUNCTION(BlueprintPure, Category = "Spells|Mana") FORCEINLINE
	float GetMaximumMana() const { return MaxMana; }

	UFUNCTION(BlueprintCallable, Category = "Spells|Rage")
	void ApplyRageChange(float Delta, AActor* InstigatorActor, const FHitResult& Hit);

	UFUNCTION(BlueprintPure, Category = "Spells|Rage") FORCEINLINE
	float GetCurrentRage() const { return Rage; }

	UFUNCTION(BlueprintPure, Category = "Spells|Rage") FORCEINLINE
	float GetMaximumRage() const { return MaxRage; }

protected:
	UPhotonJSON* GetAttributesForReplication();

	// send the attributes as player or room properties depending on the owner
	void ReplicateAttributes();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, BlueprintAssignable, Category = "Spells|Health")
	FSOnAttributeChanged OnHealthAttributeChanged;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, BlueprintAssignable, Category = "Spells|Mana")
	FSOnAttributeChanged OnManaAttributeChanged;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, BlueprintAssignable, Category = "Spells|Mana")
	FSOnAttributeChanged OnRageAttributeChanged;
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spells|Attributes")
	float Health = 100.0f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Spells|Attributes|Photon Cloud")
	float LastHealth = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spells|Attributes")
	float MaxHealth = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spells|Attributes")
	float Mana = 200.0f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Spells|Attributes|Photon Cloud")
	float LastMana = 200.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spells|Attributes")
	float MaxMana = 200.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spells|Attributes")
	float Rage = 0.0f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Spells|Attributes|Photon Cloud")
	float LastRage = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spells|Attributes", meta = (ClampMin=0.0f, ClampMax=300.0f, UIMin=0.0f, UIMax=300.0f))
	float MaxRage = 300.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spells|Attributes")
	bool bUseMana = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spells|Attributes")
	bool bUseRage = false;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Spells|Photon Cloud")
	USPhotonCloudObject* PhotonCloudObject = nullptr;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Spells|Photon Cloud", meta = (AllowPrivateAccess = true, Tooltip = "Player number or actor unique id of the owner of this component"))
	int32 OwnerUniqueNetId = -1;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Spells|Photon Cloud", meta = (AllowPrivateAccess = true, Tooltip = "Player number or actor unique id of the owner of this component"))
	ESInstigatorTypes OwnerInstigatorType = ESInstigatorTypes::INVALID;
};
