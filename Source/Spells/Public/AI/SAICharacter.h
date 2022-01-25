// Spells - xixgames - juaxix - 2021/2022

#pragma once

#include "CoreMinimal.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/Character.h"
#include "SAICharacter.generated.h"

/**
 * Pawn sensing is the basic (original) version of the AI perception
 */
class UPawnSensingComponent;

class USActionsComponent;
class USAttributesComponent;
class USWorldUserWidget;

UENUM(BlueprintType)
enum class ESAIAggroLevels : uint8
{
	IDLE,
	SPOTTED,
	ATTACKING
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSOnEnemyAggroChanged, AActor*, InstigatorActor, ESAIAggroLevels, AggroLevel);

UCLASS()
class SPELLS_API ASAICharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASAICharacter();
	
	UFUNCTION(BlueprintPure, Category = "Spells|AI|Attack") FORCEINLINE
	FVector GetMuzzleLocation() const { return MuzzleSocket->GetSocketLocation(GetMesh()); }

	UFUNCTION(BlueprintPure, Category = "Spells|AI|Attributes") FORCEINLINE
	USAttributesComponent* GetAttributesComponent() const { return AttributesComponent; }

	UFUNCTION(BlueprintPure, Category = "Spells|AI|Actions") FORCEINLINE
	USActionsComponent* GetActionsComponent() const { return ActionsComponent; }

	UFUNCTION(NetMulticast, Unreliable, BlueprintCallable, Category = "Spells|Aggro")
	void Multicast_ApplyAggroLevelChange(AActor* InstigatorActor, ESAIAggroLevels NewAggroLevel);
	void Multicast_ApplyAggroLevelChange_Implementation(AActor* InstigatorActor, ESAIAggroLevels NewAggroLevel)
	{
		if (AggroLevel != NewAggroLevel)
		{
			AggroLevel = NewAggroLevel;
			OnEnemyAggroChanged.Broadcast(InstigatorActor, AggroLevel);
			CreateSpotWidget();
		}
	}

protected:
	virtual void BeginPlay() override;

	virtual void CreateSpotWidget();

	UFUNCTION()
	virtual void OnPawnInSight(APawn* InPawn);

	UFUNCTION()
	void OnHealthChanged(AActor* AttackerInstigatorActor, USAttributesComponent* AttributeComponent, float NewHealth, float Delta, const FHitResult& Hit);

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Spells|Projectile")
	float ProjectileDamage = 10.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Spells|Projectile")
	TArray<TSubclassOf<class USActionEffect>> ProjectileActionEffects;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Spells|Projectile")
	bool bOverrideProjectileEffects = true;

protected:
	UPROPERTY(Transient)
	USkeletalMeshSocket const* MuzzleSocket = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Spells|Components")
	UPawnSensingComponent* PawnSensingComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Spells|Components")
	USAttributesComponent* AttributesComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Spells|Components")
	USActionsComponent* ActionsComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spells|AI", meta = (ClampMin=1.0f, ClampMax=100.0f, UIMin=1.0f, UIMax=100.0f))
	float DestroyAfterKillSeconds = 9.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spells|UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<USWorldUserWidget> SpottedWidgetClass;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Spells|UI", meta = (AllowPrivateAccess = "true"))
	USWorldUserWidget* SpottedWidget = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, BlueprintAssignable, Category = "Spells|Aggro")
	FSOnEnemyAggroChanged OnEnemyAggroChanged;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spells|Aggro")
	ESAIAggroLevels AggroLevel = ESAIAggroLevels::IDLE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spells|Debug", meta = (AllowPrivateAccess = "true"))
	uint8 bDebug:1;
};
