// Spells - xixgames - juaxix - 2021/2022

#include "Player/SCharacter.h"

// Unreal includes
#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#if !UE_BUILD_SHIPPING
#include "DrawDebugHelpers.h"
#endif
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"

// Spells includes
#include "Gameplay/SActionsComponent.h"
#include "Gameplay/SCharacterInteractionComponent.h"
#include "Gameplay/SAttributesComponent.h"

namespace
{
	const FName SMOVEFORWARD_AXIS = TEXT("MoveForward");
	const FName SMOVERIGHT_AXIS = TEXT("MoveRight");
	const FName SLOOKUP_AXIS = TEXT("LookUp");
	const FName STURN_AXIS = TEXT("Turn");
	const FName SPRIMARY_ATTACK_KEY = TEXT("PrimaryAttack");
	const FName SSECUNDARY_ATTACK_KEY = TEXT("SecundaryAttack");
	const FName SSPECIAL_ATTACK_KEY = TEXT("SpecialAttack");
	const FName SDEFENSE_KEY = TEXT("Defense");
	const FName SPRIMARY_ACTION_KEY = TEXT("PrimaryAction");
	const FName SJUMP_KEY = TEXT("Jump");
	const FName SSPRINT_KEY = TEXT("Sprint");
	constexpr float DebugDrawScale = 100.0f;
	constexpr float DebugDrawThickness = 5.0f;
	constexpr float DebugDrawDistance = 100.0f;
}

ASCharacter::ASCharacter()
	: bDebugMode(false)
{
	PrimaryActorTick.bCanEverTick = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	AttributesComponent = CreateDefaultSubobject<USAttributesComponent>("AttributesComponent");
	ActionsComponent = CreateDefaultSubobject<USActionsComponent>("ActionsComponent");
	CharacterInteractionComponent = CreateDefaultSubobject<USCharacterInteractionComponent>(TEXT("InteractionComponent"));

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

	PlayerInputComponent->BindAction(SPRIMARY_ATTACK_KEY, IE_Pressed, this, &ASCharacter::PrimaryAttackInputActionStarts);
	PlayerInputComponent->BindAction(SPRIMARY_ATTACK_KEY, IE_Released, this, &ASCharacter::PrimaryAttackInputActionStops);
	PlayerInputComponent->BindAction(SSECUNDARY_ATTACK_KEY, IE_Pressed, this, &ASCharacter::SecundaryAttackInputAction);
	PlayerInputComponent->BindAction(SSPECIAL_ATTACK_KEY, IE_Pressed, this, &ASCharacter::SpecialAttackInputAction);
	PlayerInputComponent->BindAction(SDEFENSE_KEY, IE_Pressed, this, &ASCharacter::DefenseInputAction);

	PlayerInputComponent->BindAction(SJUMP_KEY, IE_Pressed, this, &ASCharacter::Jump);

	PlayerInputComponent->BindAction(SSPRINT_KEY, IE_Pressed, this, &ASCharacter::SprintStart);
	PlayerInputComponent->BindAction(SSPRINT_KEY, IE_Released, this, &ASCharacter::SprintStop);

	PlayerInputComponent->BindAction(SPRIMARY_ACTION_KEY, IE_Pressed, CharacterInteractionComponent, &USCharacterInteractionComponent::PrimaryAction);
}

void ASCharacter::OnHealthChanged(AActor* InstigatorActor, USAttributesComponent* OwningAttributesComp, float NewHealth, float Delta, const FHitResult& Hit)
{
	if (Delta < 0.0f)
	{
		APlayerController* PlayerController = GetController<APlayerController>();
		if (NewHealth <= 0.0f)
		{
			DisableInput(PlayerController);
			SetCanBeDamaged(false);
			
			/* TODO -- remote player health bar
			if (IsValid(WorldHealthBar))
			{
				WorldHealthBar->RemoveFromParent();
				WorldHealthBar = nullptr;
			}*/
		}
		else 
		{
			if (IsValid(PlayerController))
			{
				/* TODO -- remote player health bar
				if (!IsValid(WorldHealthBar) && WorldHealthBarClass)
				{
					WorldHealthBar = CreateWidget<USWorldUserWidget>(PlayerController, WorldHealthBarClass);
					check(WorldHealthBar);
					WorldHealthBar->AttachedActor = this;
					WorldHealthBar->AddToViewport();
				}*/

				if (CameraShake)
				{
					PlayerController->ClientStartCameraShake(CameraShake, CameraShakeScale);
				}
			}
		}
	}
}

void ASCharacter::SprintStart()
{
	ActionsComponent->StartActionByName(this, SSPRINT_KEY);
}

void ASCharacter::SprintStop()
{
	ActionsComponent->StopActionByName(this, SSPRINT_KEY);
}

#if !UE_BUILD_SHIPPING
void ASCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	DrawDebug();
}
#endif

void ASCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (!HasAnyFlags(RF_DefaultSubObject|RF_ClassDefaultObject|RF_Transient) &&
		!AttributesComponent->OnHealthAttributeChanged.IsAlreadyBound(this, &ASCharacter::OnHealthChanged))
	{
		AttributesComponent->OnHealthAttributeChanged.AddDynamic(this, &ASCharacter::OnHealthChanged);
	}
}


#if !UE_BUILD_SHIPPING
void ASCharacter::DrawDebug()
{
	const FVector DrawLineStart = GetActorLocation() + GetActorRightVector() * DebugDrawDistance;
	const FVector DrawDirection_LineEnd = DrawLineStart + (GetActorForwardVector() * DebugDrawDistance);
	DrawDebugDirectionalArrow(GetWorld(), DrawLineStart, DrawDirection_LineEnd, DebugDrawScale, FColor::Yellow, false, 0.0f, 0, DebugDrawThickness);

	const FVector ControllerDirection_LineEnd = DrawLineStart + (GetControlRotation().Vector() * 100.0f);
	DrawDebugDirectionalArrow(GetWorld(), DrawLineStart, ControllerDirection_LineEnd, DebugDrawScale, FColor::Green, false, 0.0f, 0, DebugDrawThickness);
}
#endif
