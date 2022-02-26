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
#include "Kismet/KismetMathLibrary.h"

// Photon includes
#include "PhotonCloudSubsystem.h"
#include "PhotonJSON.h"

// Spells includes
#include "Gameplay/SActionsComponent.h"
#include "Gameplay/SCharacterInteractionComponent.h"
#include "Gameplay/SAttributesComponent.h"
#include "Online/SPhotonCloudObject.h"
#include "Player/SAnimInstance.h"

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

TArray<FString> ASCharacter::AllPhotonPlayerProperties = {
	SpellsKeysForReplication::Actions,
	SpellsKeysForReplication::Attributes,
	SpellsKeysForReplication::InAir
};

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

void ASCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (!PhotonCloudObject)
	{
		PhotonCloudObject = Cast<USPhotonCloudObject>(GetGameInstance()->GetSubsystem<UPhotonCloudSubsystem>()->GetPhotonCloudAPI());
	}
	LastLocation = GetActorLocation();
	LastRotation = GetActorRotation();
	AnimInstance = Cast<USAnimInstance>(GetMesh()->GetAnimInstance());
	ActionsComponent->SetupPhoton(PlayerNumber, ESInstigatorTypes::PLAYER);
	AttributesComponent->SetupPhoton(PlayerNumber, ESInstigatorTypes::PLAYER);
	ActionsComponent->CreateDefaultActions();
}

void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	check(CharacterInteractionComponent);
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
			ACharacter::GetMovementComponent()->StopMovementImmediately();
		}
		else 
		{
			if (IsValid(PlayerController))
			{
				if (CameraShake)
				{
					PlayerController->ClientStartCameraShake(CameraShake, CameraShakeScale);
				}

				if (InstigatorActor != this) // dont rage myself
				{
					// Add Rage from a fraction of the incoming damage
					AttributesComponent->ApplyRageChange(FMath::Abs(Delta) * DamageReceivedToRageFraction, this, Hit);
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

void ASCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!bIsLocalPlayer)
	{
		LagFreeRemotePlayerSync(DeltaSeconds);
	}

#if !UE_BUILD_SHIPPING
	if (bDebugMode)
	{
		DrawDebug();
	}
#endif
}


void ASCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	if (!HasAnyFlags(RF_DefaultSubObject|RF_ClassDefaultObject) &&
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

void ASCharacter::SetupPhotonPlayer_Implementation(int32 InPlayerNumber, const FString& InPlayerName, bool bIsLocal)
{
	if (PlayerNumber == InPlayerNumber)
	{
		// already setup!
		return;
	}

	if (!PhotonCloudObject)
	{
		PhotonCloudObject = Cast<USPhotonCloudObject>(GetGameInstance()->GetSubsystem<UPhotonCloudSubsystem>()->GetPhotonCloudAPI());
	}

	PlayerNumber = InPlayerNumber;
	PlayerName = InPlayerName;
	bIsLocalPlayer = bIsLocal;

	AnimInstance = Cast<USAnimInstance>(GetMesh()->GetAnimInstance());
	if (AnimInstance)
	{
		AnimInstance->bIsLocalPlayer = bIsLocalPlayer;
	}

	if (bIsLocalPlayer)
	{
		FTimerManager& TimerManager = GetWorldTimerManager();
		TimerManager.ClearTimer(ReplicationTimerHandle);
		ReplicationTimerDelegate.Unbind();
		ReplicationTimerDelegate.BindUObject(this, &ASCharacter::Replicate_Movement, false);
		TimerManager.SetTimer(ReplicationTimerHandle, ReplicationTimerDelegate, PhotonCloudObject->Player_SyncFreq, true);
		ActionsComponent->SetupPhoton(PlayerNumber, ESInstigatorTypes::PLAYER);
		AttributesComponent->SetupPhoton(PlayerNumber, ESInstigatorTypes::PLAYER);
		ActionsComponent->CreateDefaultActions();
		
	}
	else
	{
		CharacterInteractionComponent->DestroyComponent();
	}
}

void ASCharacter::ReceivedPlayerProperties_Implementation(UPhotonJSON* ChangesJSON)
{
	if (IsValid(ChangesJSON))
	{
		if (IsValid(AnimInstance) && ChangesJSON->Contains(SpellsKeysForReplication::InAir))
		{
			AnimInstance->bIsInAir = ChangesJSON->GetBoolean(SpellsKeysForReplication::InAir);
		}

		if (ChangesJSON->Contains(SpellsKeysForReplication::Actions))
		{
			ActionsComponent->OnActionsPropertiesChanged(ChangesJSON->Get_JSON_Object(SpellsKeysForReplication::Actions));
		}

		if (ChangesJSON->Contains(SpellsKeysForReplication::Attributes))
		{
			AttributesComponent->OnAttributesPropertiesChanged(ChangesJSON->Get_JSON_Object(SpellsKeysForReplication::Attributes));
		}
	}
}

void ASCharacter::ReceivedPlayerLocationRotationControl_Implementation(const FVector& InLocation, const FRotator& InRotation, const FRotator& InControlRot)
{
	LastLocation = InLocation;
	LastRotation = InRotation;
	LastControlRot = InControlRot;
	LastMovementReceived = GetWorld()->TimeSeconds;
}

void ASCharacter::ReceivedPlayerData_Implementation(UPhotonJSON* InDataJSON)
{
	if (!InDataJSON)
	{
		return;
	}

	if (InDataJSON->Contains(SpellsKeysForReplication::ActionEvent))
	{
		ActionsComponent->OnActionsPropertiesChanged(InDataJSON->Get_JSON_Object(SpellsKeysForReplication::ActionEvent));
	}
}

void ASCharacter::Replicate_Movement(bool bForce)
{
	const FVector Location = GetActorLocation();
	const FRotator Rotation = GetActorRotation();
	const FRotator ControlRot = GetControlRotation();

	if (bForce ||
		!Location.Equals(LastLocation, 1.0f) ||
		!Rotation.Equals(LastRotation, 0.1f) ||
		!ControlRot.Equals(LastControlRot, 0.2f))
	{
		static TArray<int32> NoTarget;
		LastLocation = Location;
		LastRotation = Rotation;
		LastControlRot = ControlRot;
		PhotonCloudObject->SendPlayerLocationRotationScale(LastLocation, LastRotation, LastControlRot.Euler(), NoTarget, false);
	}

	if (bForce || AnimInstance->bIsInAir != bLastIsInAir)
	{
		bLastIsInAir = AnimInstance->bIsInAir;
		UPhotonJSON* JumpJSON = UPhotonJSON::Create(this);
		JumpJSON->SetBoolean(SpellsKeysForReplication::InAir, bLastIsInAir);
		PhotonCloudObject->SetPlayerCustomProperties(JumpJSON);
	}
}

void ASCharacter::LagFreeRemotePlayerSync(float DeltaSeconds)
{
	static const float LastMovementTimeout = PhotonCloudObject->Player_SyncFreq * 3;
	if (LastMovementReceived > 0)
	{
		if (GetWorld()->TimeSeconds - LastMovementReceived < LastMovementTimeout)
		{
			const float DeltaTime = DeltaSeconds * PhotonCloudObject->GetRoundTripTime();
			const FVector Location = GetActorLocation();
			if (AnimInstance)
			{
				AnimInstance->CalculatedSpeed = FVector::Dist(Location, LastLocation) / PhotonCloudObject->Player_SyncFreq;
			}
			SetActorLocation(UKismetMathLibrary::VLerp(Location, LastLocation, DeltaTime));
			SetActorRotation(UKismetMathLibrary::RLerp(GetActorRotation(), LastRotation, DeltaTime, true));
		}
		else
		{
			LastMovementReceived = 0.0f;
			if (AnimInstance)
			{
				AnimInstance->CalculatedSpeed = 0.0f;
			}
		}
	}
}
