// Spells - xixgames - juaxix - 2021

#include "Player/SCharacter.h"

#include "Attacks/SMagicProjectile.h"
#include "Camera/CameraComponent.h" 
#include "GameFramework/CharacterMovementComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/SpringArmComponent.h"

namespace
{
	const FName SMOVEFORWARD_AXIS = TEXT("MoveForward");
	const FName SMOVERIGHT_AXIS = TEXT("MoveRight");
	const FName SLOOKUP_AXIS = TEXT("LookUp");
	const FName STURN_AXIS = TEXT("Turn");
	const FName SPRIMARY_ATTACK_KEY = TEXT("PrimaryAttack");
	const FName SJUMP_KEY = TEXT("Jump");
	const FName SRIGHT_HAND_SOCKET = TEXT("Muzzle_01");
	constexpr float DebugDrawScale = 100.0f;
	constexpr float DebugDrawThickness = 5.0f;
	constexpr float DebugDrawDistance = 100.0f;
}

ASCharacter::ASCharacter()
	: PrimaryAttackProjectileClass(ASMagicProjectile::StaticClass())
{
	PrimaryActorTick.bCanEverTick = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));

	SpringArmComponent->SetupAttachment(RootComponent);
	CameraComponent->SetupAttachment(SpringArmComponent);

	GetCharacterMovement()->bOrientRotationToMovement = true;
	SpringArmComponent->bUsePawnControlRotation = true;
	bUseControllerRotationYaw = false;
}

void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(SMOVEFORWARD_AXIS, this, &ASCharacter::MoveForward);
	PlayerInputComponent->BindAxis(SMOVERIGHT_AXIS, this, &ASCharacter::MoveRight);

	PlayerInputComponent->BindAxis(SLOOKUP_AXIS, this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis(STURN_AXIS, this, &APawn::AddControllerYawInput);

	PlayerInputComponent->BindAction(SPRIMARY_ATTACK_KEY, IE_Pressed, this, &ASCharacter::PrimaryAttack);
	PlayerInputComponent->BindAction(SJUMP_KEY, IE_Pressed, this, &ASCharacter::Jump);
}

void ASCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	DrawDebug();
}

void ASCharacter::PrimaryAttack()
{
	ensureAlwaysMsgf(PrimaryAttackProjectileClass != nullptr, TEXT("PrimaryAttackProjectileClass needs a class"));

	// GetActorLocation() + FVector(0.f,0.f,BaseEyeHeight) // EYES
	const FTransform SpawnTransform = FTransform(GetControlRotation(), GetMesh()->GetSocketLocation(SRIGHT_HAND_SOCKET));
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	SpawnParams.Instigator = this;
	GetWorld()->SpawnActor<ASMagicProjectile>(PrimaryAttackProjectileClass, SpawnTransform, SpawnParams);
}

void ASCharacter::DrawDebug()
{
	FVector DrawLineStart = GetActorLocation() + GetActorRightVector() * DebugDrawDistance;
	FVector DrawDirection_LineEnd = DrawLineStart + (GetActorForwardVector() * DebugDrawDistance);
	DrawDebugDirectionalArrow(GetWorld(), DrawLineStart, DrawDirection_LineEnd, DebugDrawScale, FColor::Yellow, false, 0.0f, 0, DebugDrawThickness);

	FVector ControllerDirection_LineEnd = DrawLineStart + (GetControlRotation().Vector() * 100.0f);
	DrawDebugDirectionalArrow(GetWorld(), DrawLineStart, ControllerDirection_LineEnd, DebugDrawScale, FColor::Green, false, 0.0f, 0, DebugDrawThickness);
}

