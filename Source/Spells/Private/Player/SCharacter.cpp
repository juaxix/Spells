// Spells - xixgames - juaxix - 2021

#include "Player/SCharacter.h"

#include "DrawDebugHelpers.h"
#include "Attacks/SMagicProjectile.h"
#include "Camera/CameraComponent.h" 
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Gameplay/SCharacterInteractionComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/SAttributesComponent.h"

namespace
{
	const FName SMOVEFORWARD_AXIS = TEXT("MoveForward");
	const FName SMOVERIGHT_AXIS = TEXT("MoveRight");
	const FName SLOOKUP_AXIS = TEXT("LookUp");
	const FName STURN_AXIS = TEXT("Turn");
	const FName SPRIMARY_ATTACK_KEY = TEXT("PrimaryAttack");
	const FName SSECUNDARY_ATTACK_KEY = TEXT("SecundaryAttack");
	const FName SSPECIAL_ATTACK_KEY = TEXT("SpecialAttack");
	const FName SPRIMARY_ACTION_KEY = TEXT("PrimaryAction");
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
	AttributesComponent = CreateDefaultSubobject<USAttributesComponent>("AttributesComponent");

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
	PlayerInputComponent->BindAction(SSPECIAL_ATTACK_KEY, IE_Pressed, this, &ASCharacter::SpecialAttackInputAction);
	PlayerInputComponent->BindAction(SJUMP_KEY, IE_Pressed, this, &ASCharacter::Jump);
	PlayerInputComponent->BindAction(SPRIMARY_ACTION_KEY, IE_Pressed, CharacterInteractionComponent, &USCharacterInteractionComponent::PrimaryAction);
}

void ASCharacter::OnHealthChanged(AActor* InstigatorActor, USAttributesComponent* OwningAttributesComp, float NewHealth, float Delta, const FHitResult& Hit)
{
	if (Delta < 0.0f)
	{
		if (NewHealth <= 0.0f)
		{
			DisableInput(Cast<APlayerController>(GetController()));
		}
		else
		{
			UGameplayStatics::PlayWorldCameraShake(this, CameraShake, GetPawnViewLocation(), 10.0f, 0.0f);
		}
	}
}

void ASCharacter::BeginPlay()
{
	Super::BeginPlay();
	// Cache sockets
	RightHandSocket = GetMesh()->GetSocketByName(SRIGHT_HAND_SOCKET);
}

void ASCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	DrawDebug();
}

void ASCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (!HasAnyFlags(RF_DefaultSubObject|RF_ClassDefaultObject|RF_Transient) &&
		!AttributesComponent->OnHealthAttributeChanged.IsAlreadyBound(this, &ASCharacter::OnHealthChanged))
	{
		AttributesComponent->OnHealthAttributeChanged.AddDynamic(this, &ASCharacter::OnHealthChanged);
	}
}

void ASCharacter::DoMagicalAttack(TSubclassOf<ASMagicProjectile>& MagicProjectileClass)
{
	ensureAlwaysMsgf(MagicProjectileClass != nullptr, TEXT("PrimaryAttackProjectileClass needs a class"));
	FRotator ProjectileRotation;
	UWorld* World = GetWorld();
	FVector HandLocation = RightHandSocket->GetSocketLocation(GetMesh());
	
	if (APlayerController* PlayerController = Cast<APlayerController>(GetOwner()))
	{
		ProjectileRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
		switch (AimMode)
		{
			default: case ESAimModes::Viewport: break;
			case ESAimModes::HitScan:
			{
				FHitResult Hit;
				FCollisionQueryParams CollisionQueryParams;
				CollisionQueryParams.AddIgnoredActor(this);
				FVector LookStart = PlayerController->PlayerCameraManager->GetCameraLocation();
				FVector LookEnd = LookStart + PlayerController->PlayerCameraManager->GetActorForwardVector() * MaxHitScanDistanceLook;
				
				if (World->LineTraceSingleByObjectType(Hit, LookStart, LookEnd, FCollisionObjectQueryParams::AllObjects, CollisionQueryParams))
				{
					LookEnd = Hit.ImpactPoint;
					if (bDebugMode)
					{
						DrawDebugSphere(World, LookEnd, 23.f, 12, FColor::Red, false, 1.5f);
					}

					ProjectileRotation = FRotationMatrix::MakeFromX(LookEnd - HandLocation).Rotator();
				}
				
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
	const FVector DrawLineStart = GetActorLocation() + GetActorRightVector() * DebugDrawDistance;
	const FVector DrawDirection_LineEnd = DrawLineStart + (GetActorForwardVector() * DebugDrawDistance);
	DrawDebugDirectionalArrow(GetWorld(), DrawLineStart, DrawDirection_LineEnd, DebugDrawScale, FColor::Yellow, false, 0.0f, 0, DebugDrawThickness);

	const FVector ControllerDirection_LineEnd = DrawLineStart + (GetControlRotation().Vector() * 100.0f);
	DrawDebugDirectionalArrow(GetWorld(), DrawLineStart, ControllerDirection_LineEnd, DebugDrawScale, FColor::Green, false, 0.0f, 0, DebugDrawThickness);
}

