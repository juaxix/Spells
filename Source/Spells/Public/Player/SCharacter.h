// Spells - xixgames - juaxix - 2021
// Character is controlled by a player controller

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"

class ASMagicProjectile;
class UAnimMontage;
class USAttributesComponent;
class UCameraComponent;
class USCharacterInteractionComponent;
class USkeletalMeshSocket;
class USpringArmComponent;

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
	virtual void PrimaryAttackAnimNotif() { DoMagicalAttack(PrimaryAttackProjectileClass); }

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Spells|Attack")
	virtual void SecundaryAttackAnimNotif() { DoMagicalAttack(SecundaryAttackProjectileClass); }

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Spells|Attack")
	virtual void SpecialAttackAnimNotif() { DoMagicalAttack(SpecialAttackProjectileClass); }

	UFUNCTION()
	void OnHealthChanged(AActor* InstigatorActor, USAttributesComponent* OwningAttributesComp, float NewHealth, float Delta, const FHitResult& Hit);

	UFUNCTION(BlueprintPure, Category = "Spells|Player|Attributes") FORCEINLINE
	USAttributesComponent* GetAttributesComponent() const {return AttributesComponent;}

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void PostInitializeComponents() override;

	// Move in the direction of the controller
	virtual void MoveForward(float Value) { AddMovementInput(FRotator(0.0f, GetControlRotation().Yaw, 0.0f).Vector(), Value);}
	virtual void MoveRight(float Value) { AddMovementInput(FRotationMatrix(FRotator(0.0f, GetControlRotation().Yaw, 0.0f)).GetScaledAxis(EAxis::Y), Value); }

	virtual void PrimaryAttackInputAction()
	{
		PlayAnimMontage(PrimaryAttackAnimMontage);
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

	virtual void DrawDebug();

	UPROPERTY(VisibleAnywhere, Category = "Player")
	UCameraComponent* CameraComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player")
	USpringArmComponent* SpringArmComponent = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	USCharacterInteractionComponent* CharacterInteractionComponent = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Attributes")
	USAttributesComponent* AttributesComponent = nullptr;

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

	UPROPERTY(Category = "Setup", EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", ToolTip = "PFX to spawn when casting a spell"))
	TSubclassOf<class UCameraShakeBase> CameraShake;
private:
	UPROPERTY(Transient)
	USkeletalMeshSocket const* RightHandSocket = nullptr;
};
