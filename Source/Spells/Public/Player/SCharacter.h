// Spells - xixgames - juaxix - 2021/2022
// Character is controlled by a player controller

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Gameplay/SAttributesComponent.h"
#include "SCharacter.generated.h"

class ASMagicProjectile;
class UAnimMontage;
class USActionsComponent;
class USAttributesComponent;
class UCameraComponent;
class USCharacterInteractionComponent;
class USkeletalMeshSocket;
class USpringArmComponent;
class UUserWidget;
class USWorldUserWidget;

UENUM(BlueprintType)
enum class ESAimModes: uint8
{
	HitScan = 0 UMETA(Tooltip = "Trace to hit for aiming"),
	Viewport = 1 UMETA(Tooltip = "Use viewport rotation")
};

UCLASS()
class SPELLS_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASCharacter();

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Spells|Attack")
	virtual void PrimaryAttackAnimNotif()
	{
		DoMagicalAttack(PrimaryAttackProjectileClass);
		if (bPressingFire1 && AttributesComponent->IsAlive())
		{
			PrimaryAttackInputAction();
		}
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Spells|Attack")
	virtual void SecundaryAttackAnimNotif() { DoMagicalAttack(SecundaryAttackProjectileClass); }

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Spells|Attack")
	virtual void SpecialAttackAnimNotif() { DoMagicalAttack(SpecialAttackProjectileClass); }

	UFUNCTION()
	void OnHealthChanged(AActor* InstigatorActor, USAttributesComponent* OwningAttributesComp, float NewHealth, float Delta, const FHitResult& Hit);

	UFUNCTION(BlueprintPure, Category = "Spells|Player|Attributes") FORCEINLINE
	USAttributesComponent* GetAttributesComponent() const {return AttributesComponent;}

	/** 
	* Make the character sprint on the next update.	 
	*/
	UFUNCTION(BlueprintCallable, Category="Spells|Character")
	virtual void SprintStart();

	/** 
	* Make the character stops sprinting on the next update.	 
	*/
	UFUNCTION(BlueprintCallable, Category="Spells|Character")
	virtual void SprintStop();

	virtual void BeginPlay() override;

#if !UE_BUILD_SHIPPING
	virtual void Tick(float DeltaSeconds) override;
#endif

	virtual void PostInitializeComponents() override;

	// Move in the direction of the controller
	virtual void MoveForward(float Value) { AddMovementInput(FRotator(0.0f, GetControlRotation().Yaw, 0.0f).Vector(), Value);}
	virtual void MoveRight(float Value) { AddMovementInput(FRotationMatrix(FRotator(0.0f, GetControlRotation().Yaw, 0.0f)).GetScaledAxis(EAxis::Y), Value); }

	virtual void PrimaryAttackInputAction()
	{
		PlayAnimMontage(PrimaryAttackAnimMontage);
		bPressingFire1 = true;
	}

	virtual void PrimaryAttackInputActionStops()
	{
		bPressingFire1 = false;
	}

	virtual void SecundaryAttackInputAction()
	{
		PlayAnimMontage(SecundaryAttackAnimMontage);
	}

	virtual void SpecialAttackInputAction()
	{
		PlayAnimMontage(SpecialAttackAnimMontage);
	}

	virtual void DoMagicalAttack(TSubclassOf<ASMagicProjectile>& MagicProjectileClass);

#if !UE_BUILD_SHIPPING
	virtual void DrawDebug();
#endif

	UFUNCTION(Exec)
	virtual void HealSelf()
	{
#if !UE_BUILD_SHIPPING
		AttributesComponent->ApplyHealthChange(
			AttributesComponent->GetMaximumHealth()-AttributesComponent->GetCurrentHealth(), 
			this, FHitResult());
#else
		UE_LOG(LogTemp, Log, TEXT("Disabled"));
#endif
	}

public:
	UPROPERTY(VisibleAnywhere, Category = "Player")
	UCameraComponent* CameraComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player")
	USpringArmComponent* SpringArmComponent = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	USCharacterInteractionComponent* CharacterInteractionComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes")
	USAttributesComponent* AttributesComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Actions")
	USActionsComponent* ActionsComponent = nullptr;

	UPROPERTY(EditAnywhere, Category = "Attack")
	UAnimMontage* PrimaryAttackAnimMontage = nullptr;

	UPROPERTY(EditAnywhere, Category = "Attack")
	UAnimMontage* SecundaryAttackAnimMontage = nullptr;

	UPROPERTY(EditAnywhere, Category = "Attack")
	UAnimMontage* SpecialAttackAnimMontage = nullptr;

	UPROPERTY(EditAnywhere, Category = "Attack")
	TSubclassOf<ASMagicProjectile> PrimaryAttackProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Attack")
	TSubclassOf<ASMagicProjectile> SecundaryAttackProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Attack")
	TSubclassOf<ASMagicProjectile> SpecialAttackProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Setup")
	uint8 bDebugMode:1;

	UPROPERTY(EditAnywhere, Category = "Setup")
	ESAimModes AimMode = ESAimModes::Viewport;

	UPROPERTY(Category = "Setup", EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", ToolTip = "PFX to spawn when casting a spell"))
	UParticleSystem* MuzzleParticleSystem = nullptr;

	UPROPERTY(Category = "Setup", EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", ToolTip = "Camera shake to perform on receiving an attack"))
	TSubclassOf<class UCameraShakeBase> CameraShake;

	UPROPERTY(Category = "Setup", EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", ToolTip = "Camera shake intensity"))
	int32 CameraShakeScale = 10;

	uint8 bPressingFire1:1;

	/* TODO -- remote player health bar
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "UI")
	USWorldUserWidget* WorldHealthBar = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> WorldHealthBarClass = nullptr;
	*/
private:
	UPROPERTY(Transient)
	USkeletalMeshSocket const* RightHandSocket = nullptr;
};
