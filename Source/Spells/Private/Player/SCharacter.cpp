// Spells - xixgames - juaxix - 2021

#include "Player/SCharacter.h"

#include "DrawDebugHelpers.h"
#include "Attacks/SMagicProjectile.h"
#include "Camera/CameraComponent.h" 
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Gameplay/SCharacterInteractionComponent.h"

namespace
{
	const FName SMOVEFORWARD_AXIS = TEXT("MoveForward");
	const FName SMOVERIGHT_AXIS = TEXT("MoveRight");
	const FName SLOOKUP_AXIS = TEXT("LookUp");
	const FName STURN_AXIS = TEXT("Turn");
	const FName SPRIMARY_ATTACK_KEY = TEXT("PrimaryAttack");
	const FName SSECUNDARY_ATTACK_KEY = TEXT("SecundaryAttack");
	const FName SPRIMARY_ACTION_KEY = TEXT("PrimaryAction");
	const FName SSECUNDARY_ACTION_KEY = TEXT("SecundaryAction");
	const FName SJUMP_KEY = TEXT("Jump");
	const FName SRIGHT_HAND_SOCKET = TEXT("Muzzle_01");
	constexpr float MaxHitScanDistanceLook = 10000.0f;
	constexpr float DebugDrawScale = 100.0f;
	constexpr float DebugDrawThickness = 5.0f;
	constexpr float DebugDrawDistance = 100.0f;
}

ASCharacter::ASCharacter()
	: PrimaryAttackProjectileClass(ASMagicProjectile::StaticClass())
	, bDebugMode(false)
{
	PrimaryActorTick.bCanEverTick = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));

	SpringArmComponent->SetupAttachment(RootComponent);
	CameraComponent->SetupAttachment(SpringArmComponent);

	CharacterInteractionComponent = CreateDefaultSubobject<USCharacterInteractionComponent>(TEXT("InteractionComponent"));

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

	PlayerInputComponent->BindAction(SPRIMARY_ATTACK_KEY, IE_Pressed, this, &ASCharacter::PrimaryAttackInputAction);
	PlayerInputComponent->BindAction(SSECUNDARY_ATTACK_KEY, IE_Pressed, this, &ASCharacter::SecundaryAttackInputAction);
	PlayerInputComponent->BindAction(SJUMP_KEY, IE_Pressed, this, &ASCharacter::Jump);
	PlayerInputComponent->BindAction(SPRIMARY_ACTION_KEY, IE_Pressed, CharacterInteractionComponent, &USCharacterInteractionComponent::PrimaryAction);
}

void ASCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	DrawDebug();
}

void ASCharacter::DoMagicalAttack(TSubclassOf<ASMagicProjectile>& MagicProjectileClass)
{
	ensureAlwaysMsgf(MagicProjectileClass != nullptr, TEXT("PrimaryAttackProjectileClass needs a class"));
	FRotator ProjectileRotation;
	UWorld* World = GetWorld();
	FVector HandLocation = GetMesh()->GetSocketLocation(SRIGHT_HAND_SOCKET);
	if (APlayerController* PlayerController = Cast<APlayerController>(GetOwner()))
	{
		ProjectileRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
		switch(AimMode)
		{
			default: case ESAimModes::Viewport: break;
			case ESAimModes::HitScan:
			{
				FHitResult Hit;
				FCollisionQueryParams CollisionQueryParams;
				CollisionQueryParams.AddIgnoredActor(this);
				FVector LookStart = PlayerController->PlayerCameraManager->GetCameraLocation();
				FVector LookEnd = LookStart + PlayerController->PlayerCameraManager->GetActorForwardVector() * MaxHitScanDistanceLook;
				DrawDebugLine(World, LookStart, LookEnd, FColor::Red, false, 2.0f);
				if (World->LineTraceSingleByChannel(Hit, LookStart, LookEnd,ECC_Visibility, CollisionQueryParams))
				{
					LookEnd = Hit.ImpactPoint;
				}
				
				ProjectileRotation = FRotationMatrix::MakeFromX(LookEnd - HandLocation).Rotator();
				
				break;
			}
		}
	}
	else
	{
		ProjectileRotation = GetControlRotation();
	}

	const FTransform SpawnTransform = FTransform(ProjectileRotation, HandLocation);
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	SpawnParams.Instigator = this;

	World->SpawnActor<ASMagicProjectile>(MagicProjectileClass, SpawnTransform, SpawnParams);
}

void ASCharacter::DrawDebug()
{
	FVector DrawLineStart = GetActorLocation() + GetActorRightVector() * DebugDrawDistance;
	FVector DrawDirection_LineEnd = DrawLineStart + (GetActorForwardVector() * DebugDrawDistance);
	DrawDebugDirectionalArrow(GetWorld(), DrawLineStart, DrawDirection_LineEnd, DebugDrawScale, FColor::Yellow, false, 0.0f, 0, DebugDrawThickness);

	FVector ControllerDirection_LineEnd = DrawLineStart + (GetControlRotation().Vector() * 100.0f);
	DrawDebugDirectionalArrow(GetWorld(), DrawLineStart, ControllerDirection_LineEnd, DebugDrawScale, FColor::Green, false, 0.0f, 0, DebugDrawThickness);
}

