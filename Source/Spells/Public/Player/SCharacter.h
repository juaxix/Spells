// Spells - xixgames - juaxix - 2021
// Character is controlled by a player controller

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class ASMagicProjectile;

UCLASS()
class SPELLS_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASCharacter();

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void Tick(float DeltaSeconds) override;

	// Move in the direction of the controller
	virtual void MoveForward(float Value) { AddMovementInput(FRotator(0.0f, GetControlRotation().Yaw, 0.0f).Vector(), Value);}
	virtual void MoveRight(float Value) { AddMovementInput(FRotationMatrix(FRotator(0.0f, GetControlRotation().Yaw, 0.0f)).GetScaledAxis(EAxis::Y), Value); }

	virtual void PrimaryAttack();

	virtual void DrawDebug();

	UPROPERTY(VisibleAnywhere, Category = "Player")
	UCameraComponent* CameraComponent = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Player")
	USpringArmComponent* SpringArmComponent = nullptr;

	UPROPERTY(EditAnywhere, Category = "Magic")
	TSubclassOf<ASMagicProjectile> PrimaryAttackProjectileClass;
};
