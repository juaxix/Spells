// Spells - xixgames - juaxix - 2021/2022
// Character is controlled by a player controller

#pragma once

// Unreal includes
#include "CoreMinimal.h"
#include "GameFramework/Character.h"

// Spells includes
#include "Gameplay/SActionsComponent.h"
#include "Gameplay/SAttributesComponent.h"

#include "SCharacter.generated.h"

class USCharacterInteractionComponent;
class USpringArmComponent;
class UCameraComponent;

UCLASS(BlueprintType, Blueprintable)
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
		ActionsComponent->ReceiveAnimNotif(this, MagicMissileActionName);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Spells|Attack")
	virtual void SecundaryAttackAnimNotif()
	{
		ActionsComponent->ReceiveAnimNotif(this, TeleportActionName);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Spells|Attack")
	virtual void SpecialAttackAnimNotif()
	{
		ActionsComponent->ReceiveAnimNotif(this, BlackholeActionName);
	}
	
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

#if !UE_BUILD_SHIPPING
	virtual void Tick(float DeltaSeconds) override;
#endif

	virtual void PostInitializeComponents() override;

	// Move in the direction of the controller
	virtual void MoveForward(float Value) { AddMovementInput(FRotator(0.0f, GetControlRotation().Yaw, 0.0f).Vector(), Value);}
	virtual void MoveRight(float Value) { AddMovementInput(FRotationMatrix(FRotator(0.0f, GetControlRotation().Yaw, 0.0f)).GetScaledAxis(EAxis::Y), Value); }

	virtual void PrimaryAttackInputAction()
	{
		ActionsComponent->StartActionByName(this, MagicMissileActionName);
	}

	virtual void PrimaryAttackInputActionStops()
	{
		ActionsComponent->StopActionByName(this, MagicMissileActionName);
	}

	virtual void SecundaryAttackInputAction()
	{
		ActionsComponent->StartActionByName(this, TeleportActionName);
	}

	virtual void SpecialAttackInputAction()
	{
		ActionsComponent->StartActionByName(this, BlackholeActionName);
	}
	
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
	

	UPROPERTY(EditAnywhere, Category = "Setup")
	uint8 bDebugMode:1;

	UPROPERTY(Category = "Setup", EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", ToolTip = "PFX to spawn when casting a spell"))
	UParticleSystem* MuzzleParticleSystem = nullptr;

	UPROPERTY(Category = "Setup", EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", ToolTip = "Camera shake to perform on receiving an attack"))
	TSubclassOf<class UCameraShakeBase> CameraShake;

	UPROPERTY(Category = "Setup", EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", ToolTip = "Camera shake intensity"))
	int32 CameraShakeScale = 10;

	const FName MagicMissileActionName = "MagicMissile";
	const FName TeleportActionName = "Teleport";
	const FName BlackholeActionName = "Blackhole";
	
	/* TODO -- remote player health bar
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "UI")
	USWorldUserWidget* WorldHealthBar = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> WorldHealthBarClass = nullptr;
	*/

};
