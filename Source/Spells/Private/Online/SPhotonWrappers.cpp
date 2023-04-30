// Spells - xixgames - juaxix - 2021/2023

#include "Online/SPhotonWrappers.h"

// Engine includes
#include "Kismet/KismetGuidLibrary.h"
#include "Kismet/KismetStringLibrary.h"

// Photon includes
#include "PhotonCloudConfig.h"
#include "PhotonCloudSubsystem.h"

// Spells includes
#include "SGameInstance.h"

void USPhotonConnectAsyncCall::Activate()
{
	UPhotonCloudObject* PhotonCloudObject = GetOuter()->GetWorld()->GetGameInstance()->GetSubsystem<UPhotonCloudSubsystem>()->GetPhotonCloudAPI();
	PhotonCloudObject->GetPhotonCloudConfig()->userID = UKismetSystemLibrary::GetPlatformUserName() + TEXT("-") +
		UKismetStringLibrary::GetSubstring(UKismetGuidLibrary::NewGuid().ToString(), 0, 6);
	PhotonCloudObject->Disconnect();
	PhotonCloudObject->OnPhotonCloudConnected.AddDynamic(this, &USPhotonConnectAsyncCall::Connected);
	PhotonCloudObject->OnPhotonConnectionError.AddDynamic(this, &USPhotonConnectAsyncCall::ErrorConnecting);
	PhotonCloudObject->Connect();
}

void USPhotonConnectAsyncCall::SetReadyToDestroy()
{
	OnCompleted.RemoveAll(this);
	UPhotonCloudObject* PhotonCloudObject = GetOuter()->GetWorld()->GetGameInstance()->GetSubsystem<UPhotonCloudSubsystem>()->GetPhotonCloudAPI();
	PhotonCloudObject->FetchServerTimeStamp();
	PhotonCloudObject->OnPhotonCloudConnected.RemoveAll(this);
	PhotonCloudObject->OnPhotonConnectionError.RemoveAll(this);

	Super::SetReadyToDestroy();
}

void USPhotonJoinOrCreateRoomAsyncCall::Activate()
{
	UPhotonCloudObject* PhotonCloudObject = GetOuter()->GetWorld()->GetGameInstance()->GetSubsystem<UPhotonCloudSubsystem>()->GetPhotonCloudAPI();
	if (PhotonCloudObject->GetState() == EPhotonCloudStates::JOINED)
	{
		OnCompleted.Broadcast(false, EPhotonErrorCode::ALREADY_MATCHED, TEXT("Photon is already connected and joined a room"), -1, TEXT(""), false);
		return;
	}

	if (PhotonCloudObject->GetState() == EPhotonCloudStates::UNINITIALIZED)
	{
		OnCompleted.Broadcast(false, EPhotonErrorCode::INVALID_AUTHENTICATION, TEXT("Photon needs a connection before trying to join a room!"), -1, TEXT(""), false);
		return;
	}

	PhotonCloudObject->OnPlayerJoinedRoom.AddDynamic(this, &USPhotonJoinOrCreateRoomAsyncCall::CreateRoomSuccess);
	PhotonCloudObject->OnJoinOrCreateRoomError.AddDynamic(this, &USPhotonJoinOrCreateRoomAsyncCall::CreateRoomError);
	if (bOnlyCreate)
	{
		PhotonCloudObject->CreateRoom(RoomName, MaxPlayers, InitialProperties);
	}
	else
	{
		PhotonCloudObject->JoinOrCreateRoom(RoomName, MaxPlayers, InitialProperties);
	}
}

void USPhotonJoinOrCreateRoomAsyncCall::SetReadyToDestroy()
{
	UPhotonCloudObject* PhotonCloudObject = GetOuter()->GetWorld()->GetGameInstance()->GetSubsystem<UPhotonCloudSubsystem>()->GetPhotonCloudAPI();
	PhotonCloudObject->OnPlayerJoinedRoom.RemoveAll(this);
	PhotonCloudObject->OnPhotonConnectionError.RemoveAll(this);

	Super::SetReadyToDestroy();
}

void USPhotonJoinRandomRoomAsyncCall::Activate()
{
	UPhotonCloudObject* PhotonCloudObject = GetOuter()->GetWorld()->GetGameInstance()->GetSubsystem<UPhotonCloudSubsystem>()->GetPhotonCloudAPI();
	if (PhotonCloudObject->GetState() == EPhotonCloudStates::JOINED)
	{
		OnCompleted.Broadcast(false, EPhotonErrorCode::ALREADY_MATCHED, TEXT("Photon is already connected and joined a room"), -1, TEXT(""), false);
		return;
	}

	if (PhotonCloudObject->GetState() == EPhotonCloudStates::UNINITIALIZED)
	{
		OnCompleted.Broadcast(false, EPhotonErrorCode::INVALID_AUTHENTICATION, TEXT("Photon needs a connection before trying to join a room!"), -1, TEXT(""), false);
		return;
	}

	PhotonCloudObject->OnPlayerJoinedRoom.AddDynamic(this, &USPhotonJoinRandomRoomAsyncCall::JoinedRoomSuccess);
	PhotonCloudObject->OnJoinOrCreateRoomError.AddDynamic(this, &USPhotonJoinRandomRoomAsyncCall::JoinRoomError);
	PhotonCloudObject->JoinRandomRoom(LobbyName, LobbyType, MaxPlayers, MatchMakingMode, InitialProperties, LobbySQL);
	
}

void USPhotonJoinRandomRoomAsyncCall::SetReadyToDestroy()
{
	UPhotonCloudObject* PhotonCloudObject = GetOuter()->GetWorld()->GetGameInstance()->GetSubsystem<UPhotonCloudSubsystem>()->GetPhotonCloudAPI();
	PhotonCloudObject->OnPlayerJoinedRoom.RemoveAll(this);
	PhotonCloudObject->OnJoinOrCreateRoomError.RemoveAll(this);

	Super::SetReadyToDestroy();
}
