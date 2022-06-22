// Spells - xixgames - juaxix - 2021/2022

#include "AI/SAICharacter.h"

// Unreal includes
#include "Components/CapsuleComponent.h"
#include "BrainComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Perception/PawnSensingComponent.h"

// Photon includes
#include "PhotonCloudAPIBPLibrary.h"
#include "PhotonJSON.h"

// Spells game includes
#include "AI/SAIController.h"
#include "Gameplay/SActionsComponent.h"
#include "Gameplay/SAttributesComponent.h"
#include "Player/SCharacter.h"
#include "UI/SWorldUserWidget.h"

namespace
{
	const FName SAI_MUZZLE_NAME = TEXT("Muzzle_01");
}

ASAICharacter::ASAICharacter()
	: bInitialSync(false)
	, bDebug(false)
{
	PrimaryActorTick.bCanEverTick = true;
	PawnSensingComponent = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComponent"));
	AttributesComponent = CreateDefaultSubobject<USAttributesComponent>(TEXT("AttributesComponent"));
	ActionsComponent = CreateDefaultSubobject<USActionsComponent>("ActionsComponent");
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);
}

float ASAICharacter::GetCurrentSpeed() const
{
	if (PhotonCloudObject && PhotonCloudObject->AmIMaster())
	{
		return GetVelocity().Size();
	}

	return LastSpeed;
}

void ASAICharacter::OnReceivedActorLocationRotation(const FVector& NewLocation, const FRotator& NewRotation)
{
	LastLocation = NewLocation;
	LastRotation = NewRotation;
	LastMovementSync = GetWorld()->TimeSeconds;
	LastSpeed = FVector::Dist(GetActorLocation(), LastLocation) / PhotonCloudObject->AI_SyncFreq;
}

void ASAICharacter::SetupPhoton(USPhotonCloudObject* InPhotonCloudObject, int32 EnemyUniqueId)
{
	PhotonCloudObject = InPhotonCloudObject;
	AIUniqueId = EnemyUniqueId;
	HashedName = UPhotonCloudAPIBPLibrary::GetHashedName(FString::Printf(TEXT("AI_%d"), EnemyUniqueId));
	LastLocation = GetActorLocation();
	LastRotation = GetActorRotation();
	LastMovementSync = 0.0f;
	ActionsComponent->SetupPhoton(AIUniqueId, ESInstigatorTypes::MASTER_AI);
	AttributesComponent->SetupPhoton(AIUniqueId, ESInstigatorTypes::MASTER_AI);
	ActionsComponent->CreateDefaultActions();
	if (!PhotonCloudObject->RegisterObservedMechanicsActor(HashedName, this))
	{
		UE_LOG(LogTemp, Warning, TEXT("Hashed name already registered %lld"), HashedName);
	}
}

void ASAICharacter::OnTargetActorChanged(const UPhotonJSON* TargetActorJSON)
{
	if (!IsValid(TargetActorJSON) || !PhotonCloudObject || PhotonCloudObject->AmIMaster())
	{
		return;
	}

	if (APawn* NewTargetPawn = Cast<APawn>(PhotonCloudObject->FindInstigatorWithUniqueId(
		static_cast<ESInstigatorTypes>(TargetActorJSON->GetByte(SpellsKeysForReplication::InstigatorType)),
		TargetActorJSON->GetInteger(SpellsKeysForReplication::InstigatorUniqueId))))
	{
		OnClientSpottedTarget(NewTargetPawn);
	}
}

void ASAICharacter::BeginPlay()
{
	Super::BeginPlay();

	ApplyAggroLevelChange(nullptr, ESAIAggroLevels::IDLE);

	// cache muzzle
	MuzzleSocket = GetMesh()->GetSocketByName(SAI_MUZZLE_NAME);

	BindEvents();
}

void ASAICharacter::BindEvents()
{
	if (PhotonCloudObject && PhotonCloudObject->AmIMaster())
	{
		PawnSensingComponent->bOnlySensePlayers = false;
		PawnSensingComponent->OnSeePawn.AddUniqueDynamic(this, &ASAICharacter::OnMasterPawnInSight);
	}

	AttributesComponent->OnHealthAttributeChanged.AddUniqueDynamic(this, &ASAICharacter::OnHealthChanged);
}

void ASAICharacter::OnClientSpottedTarget(APawn* InPawn)
{
	if (ASAIController* AIController = GetController<ASAIController>())
	{
		AIController->SetCurrentTargetActor(InPawn);
		if (!SpottedWidget)
		{
			SpottedWidget = CreateWidget<USWorldUserWidget>(GetWorld(), SpottedWidgetClass);
			if (SpottedWidget)
			{
				SpottedWidget->AttachedActor = this;
				SpottedWidget->AddToViewport(10); // if ZOrder > 0 place this widget over others to avoid be behind health
			}
		}

		ApplyAggroLevelChange(InPawn, ESAIAggroLevels::SPOTTED);

		if (bDebug)
		{
			DrawDebugString(GetWorld(), GetActorLocation(), FString::Printf(TEXT("PLAYER SPOTTED: %s"), *InPawn->GetName()), nullptr, FColor::Cyan, 4.0f, true);
		}
	}
}

void ASAICharacter::BeginDestroy()
{
	if (PhotonCloudObject)
	{
		PhotonCloudObject->UnregisterObservedMechanicsActor(HashedName);
	}

	Super::BeginDestroy();
}

void ASAICharacter::SyncMovementReplication(bool bForce)
{
	const FVector Location = GetActorLocation();
	const FRotator Rotation = GetActorRotation();
	if (bForce ||
		!Location.Equals(LastLocation, 0.1f) ||
		!Rotation.Equals(LastRotation, 0.2f))
	{
		static TArray<int32> NoTarget;
		LastLocation = Location;
		LastRotation = Rotation;
		PhotonCloudObject->SendActorLocationRotation(HashedName, LastLocation, LastRotation, NoTarget, false);
	}
}

void ASAICharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (PhotonCloudObject->AmIMaster())
	{
		// update movement info for master replication
		LastMovementSync += DeltaSeconds;
		if (LastMovementSync >= PhotonCloudObject->AI_SyncFreq)
		{
			SyncMovementReplication();
			LastMovementSync = 0.0f;
		}
	}
	else
	{
		// Sync movement with latest received data
		LagFreeMovementSync(DeltaSeconds);
	}
}

void ASAICharacter::LagFreeMovementSync(float DeltaSeconds)
{
	static const float LastMovementTimeout = PhotonCloudObject->AI_SyncFreq * 3;
	if (LastMovementSync > 0)
	{
		if (GetWorld()->TimeSeconds - LastMovementSync < LastMovementTimeout)
		{
			const float DeltaTime = DeltaSeconds * PhotonCloudObject->GetRoundTripTime();
			const FVector Location = GetActorLocation();
			
			SetActorLocation(UKismetMathLibrary::VLerp(Location, LastLocation, DeltaTime));
			SetActorRotation(UKismetMathLibrary::RLerp(GetActorRotation(), LastRotation, DeltaTime, true));
		}
		else
		{
			LastMovementSync = 0.0f;
			LastSpeed = 0.0f;
		}
	}
}

void ASAICharacter::OnMasterPawnInSight(APawn* InPawn)
{
	if (InPawn && HashedName != -1 && PhotonCloudObject && PhotonCloudObject->AmIMaster())
	{
		if (!InPawn->IsA(ASCharacter::StaticClass()))
		{
			return;
		}

		if (const ASAIController* AIController = GetController<ASAIController>())
		{
			if (AIController->GetCurrentTargetActor() == InPawn)
			{
				return;
			}

			OnClientSpottedTarget(InPawn);
		}
	}
}

void ASAICharacter::OnHealthChanged(AActor* AttackerInstigatorActor, USAttributesComponent* AttributeComponent, float NewHealth, float Delta, const FHitResult& Hit)
{
	if (Delta < 0.0f)
	{
		if (NewHealth <= 0.0f) //died
		{
			if (const ASAIController* AIController = GetController<ASAIController>())
			{
				if (UBrainComponent* Brain = AIController->GetBrainComponent())
				{
					Brain->StopLogic("Killed");
				}
			}

			if (IsValid(SpottedWidget))
			{
				SpottedWidget->RemoveFromParent();
			}

			USkeletalMeshComponent* SkeletalMeshComponent = GetMesh();
			SkeletalMeshComponent->SetCollisionProfileName("Ragdoll");
			SkeletalMeshComponent->SetAllBodiesSimulatePhysics(true);
			GetCharacterMovement()->DisableMovement();
			GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			SetCanBeDamaged(false);
			SetLifeSpan(DestroyAfterKillSeconds);
		}
		else
		{
			if (AttackerInstigatorActor && AttackerInstigatorActor != this && PhotonCloudObject && PhotonCloudObject->AmIMaster())
			{
				if (ASAIController* AIController = GetController<ASAIController>())
				{
					AIController->SetCurrentTargetActor(AttackerInstigatorActor);
				}
			}
		}
	}
}
