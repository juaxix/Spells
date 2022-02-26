// Spells - xixgames - juaxix - 2021/2022

#include "Online/SPhotonCloudObject.h"

// Unreal includes
#include "GameFramework/GameModeBase.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

// Photon includes
#include "PhotonJSON.h"
#include "PhotonCloudAPIBPLibrary.h"

// Spells includes
#include "AI/SAICharacter.h"
#include "Gameplay/SpellsGameModeBase.h"
#include "Player/SCharacter.h"

void USPhotonCloudObject::CreateAllCharactersFromPhotonPlayers()
{
	const int32 PlayerNumber = GetPlayerNumber();
	for (FRoomPlayer& RoomPlayer : GetRoomPlayers())
	{
		OnSpawnCharacter(RoomPlayer.Number, RoomPlayer.Name, RoomPlayer.Number == PlayerNumber);
	}
}

void USPhotonCloudObject::receivedPlayerLocationRotationScale(int32 PlayerNumber, const FVector& Location, const FRotator& Rotation, const FVector& Scale)
{
	Super::receivedPlayerLocationRotationScale(PlayerNumber, Location, Rotation, Scale);
	if (JoinedCharacters.Contains(PlayerNumber) && IsValid(JoinedCharacters[PlayerNumber]))
	{
		JoinedCharacters[PlayerNumber]->ReceivedPlayerLocationRotationControl(Location, Rotation, FRotator::MakeFromEuler(Scale));
	}
}

void USPhotonCloudObject::playerCustomPropertiesChange(int32 PlayerNumber, UPhotonJSON* ChangesJSON)
{
	Super::playerCustomPropertiesChange(PlayerNumber, ChangesJSON);
	if (JoinedCharacters.Contains(PlayerNumber) && IsValid(JoinedCharacters[PlayerNumber]))
	{
		JoinedCharacters[PlayerNumber]->ReceivedPlayerProperties(ChangesJSON);
	}
}

void USPhotonCloudObject::receivedUserData(int32 PlayerNumber, UPhotonJSON* DataJSON)
{
	Super::receivedUserData(PlayerNumber, DataJSON);
	
	if (JoinedCharacters.Contains(PlayerNumber) && IsValid(JoinedCharacters[PlayerNumber]))
	{
		JoinedCharacters[PlayerNumber]->ReceivedPlayerData(DataJSON);
	}
	else if (DataJSON)
	{
		if (DataJSON->Contains(SpellsKeysForReplication::RespawnPlayer))
		{
			OnSpawnCharacter(PlayerNumber, GetPlayerName(PlayerNumber), false);
		}
	}
}

void USPhotonCloudObject::roomCustomRoomPropertiesChanged(UPhotonJSON* ChangedProperties)
{
	Super::roomCustomRoomPropertiesChanged(ChangedProperties);
	if (ASpellsGameModeBase* SpellsGameMode = Cast<ASpellsGameModeBase>(GetWorld()->GetAuthGameMode()))
	{
		SpellsGameMode->OnCustomRoomPropertiesChanged(ChangedProperties);
	}
}

void USPhotonCloudObject::masterChanged(int32 OldMasterPlayerNumber, int32 NewMasterPlayerNumber)
{
	Super::masterChanged(OldMasterPlayerNumber, NewMasterPlayerNumber);
	if (ASpellsGameModeBase* SpellsGameMode = Cast<ASpellsGameModeBase>(GetWorld()->GetAuthGameMode()))
	{
		SpellsGameMode->OnNewMaster(OldMasterPlayerNumber, NewMasterPlayerNumber);
	}
}

void USPhotonCloudObject::receivedActorLocationRotation(int32 SenderPlayerNumber, const int64& HashedName, const FVector& Location, const FRotator& Rotation)
{
	// we can control the path of the Super::receivedActorLocationRotation using only the game mode as the only actor using this is the AI
	if (SenderPlayerNumber == GetMasterPlayerNumber())
	{
		if (ASpellsGameModeBase* SpellsGameMode = Cast<ASpellsGameModeBase>(GetWorld()->GetAuthGameMode()))
		{
			SpellsGameMode->OnReceivedActorLocationRotation(HashedName, Location, Rotation);
		}
	}
	else
	{
		Super::receivedActorLocationRotation(SenderPlayerNumber, HashedName, Location, Rotation);
	}
}

void USPhotonCloudObject::receivedSerializedActorData(int32 SenderPlayerNumber, const int64& HashedName, UPhotonJSON* Data)
{
	Super::receivedSerializedActorData(SenderPlayerNumber, HashedName, Data);
	if (SenderPlayerNumber == GetMasterPlayerNumber())
	{
		if (ASpellsGameModeBase* SpellsGameMode = Cast<ASpellsGameModeBase>(GetWorld()->GetAuthGameMode()))
		{
			SpellsGameMode->OnReceivedActorData(HashedName, Data);
		}
	}
}

void USPhotonCloudObject::GetInstigatorUniqueId(AActor* InInstigator, ESInstigatorTypes& InstigatorType, int32& InstigatorUniqueId) const
{
	if (!IsValid(InInstigator))
	{
		InstigatorType = ESInstigatorTypes::INVALID;
		InstigatorUniqueId = -1;

		return;
	}

	if (const ASCharacter* Character = Cast<ASCharacter>(InInstigator))
	{
		InstigatorType = ESInstigatorTypes::PLAYER;
		InstigatorUniqueId = Character->PlayerNumber;

		return;
	}

	if (const ASAICharacter* AICharacter = Cast<ASAICharacter>(InInstigator))
	{
		InstigatorUniqueId = AICharacter->AIUniqueId;
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Unknown instigator: %s"), *GetFullNameSafe(InInstigator));
	}

	InstigatorType = ESInstigatorTypes::MASTER_AI;
}

AActor* USPhotonCloudObject::FindInstigatorWithUniqueId(ESInstigatorTypes InstigatorType, int32 UniqueId)
{
	switch (InstigatorType)
	{
		case ESInstigatorTypes::PLAYER:
		{
			if (JoinedCharacters.Contains(UniqueId))
			{
				return JoinedCharacters[UniqueId];
			}

			return nullptr;
		}
		case ESInstigatorTypes::MASTER_AI:
		{
			if (ASpellsGameModeBase* GameMode = Cast<ASpellsGameModeBase>( GetWorld()->GetAuthGameMode()))
			{
				return GameMode->FindMasterControlledActor(UniqueId);
			}
		}
		default: break;
	}

	return nullptr;
}

void USPhotonCloudObject::OnSpawnCharacter(int32 PlayerNumber, const FString& PlayerName, bool bIsLocal)
{
	if (!GEngine)
	{
		return;
	}

	UWorld* World = GetWorld();
	const UPhotonJSON* RoomProps = GetCustomRoomProperties();
	const FString MapName = RoomProps->GetString("MapName");
	const FString CurrentMapName = UGameplayStatics::GetCurrentLevelName(World, true);
	if (!MapName.IsEmpty() && !CurrentMapName.Equals(MapName))
	{
		if (!bLoadingMap)
		{
			UGameplayStatics::OpenLevel(World, FName(MapName), true);
			bLoadingMap = true;
		}
	}
	else
	{
		bLoadingMap = false;
		ASCharacter* Character = nullptr;
		if (!bIsLocal)
		{
			TArray<AActor*> PlayerStarts;
			UGameplayStatics::GetAllActorsOfClass(World, APlayerStart::StaticClass(), PlayerStarts);
			if (PlayerStarts.Num() > 0)
			{
				const AActor* SpawnStart = PlayerStarts[(PlayerStarts.Num() - 1) % PlayerNumber];
				if (AGameModeBase* GameMode = UGameplayStatics::GetGameMode(World))
				{
					if (const TSubclassOf<APawn> PawnClass = GameMode->DefaultPawnClass)
					{
						FActorSpawnParameters ActorSpawnParameters;
						ActorSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
						ActorSpawnParameters.Instigator = nullptr;
						Character = World->SpawnActor<ASCharacter>(PawnClass, SpawnStart->GetActorTransform(), ActorSpawnParameters);
						Character->ReceivedPlayerProperties(GetPlayerCustomProperties(PlayerNumber));
					}
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("No player starts found in the level"));
			}
		}
		else if (const APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0))
		{
			Character = Cast<ASCharacter>(PC->GetPawn());
		}

		if (IsValid(Character))
		{
			Character->SetupPhotonPlayer(PlayerNumber, PlayerName, bIsLocal);
			JoinedCharacters.Add(PlayerNumber, Character);
		}
	}
}

void USPhotonCloudObject::OnDestroyCharacter(int32 PlayerNumber)
{
	if (ASCharacter** Character = JoinedCharacters.Find(PlayerNumber))
	{
		if (IsValid(*Character))
		{
			(*Character)->Destroy();
		}

		JoinedCharacters.Remove(PlayerNumber);
	}
}

void USPhotonCloudObject::SyncLocalPlayer()
{
	int32 PlayerNumber = GetPlayerNumber();
	if (JoinedCharacters.Contains(PlayerNumber))
	{
		FTimerDelegate SendLocalMovementDelegate;
		FTimerHandle SendLocalMovementHandle;
		const float Rate = Player_SyncFreq * GetRoundTripTime();
		SendLocalMovementDelegate.BindLambda([this, PlayerNumber]() -> void
		{
			if (ASCharacter** Character = JoinedCharacters.Find(PlayerNumber))
			{
				if (IsValid(*Character))
				{
					(*Character)->Replicate_Movement(true);
				}
			}
		});

		GetWorld()->GetTimerManager().SetTimer(SendLocalMovementHandle, SendLocalMovementDelegate, Rate, false);
	}
}

