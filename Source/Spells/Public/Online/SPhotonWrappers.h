// Spells - xixgames - juaxix - 2021/2022

#pragma once

// Unreal includes
#include "CoreMinimal.h"
#include "PhotonCloudTypes.h"
#include "Kismet/BlueprintAsyncActionBase.h"

#include "SPhotonWrappers.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FSOnPhotonConnectCompleted, bool, bSuccess, EPhotonErrorCode, ErrorCode, const FString&, ErrorMessage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FSOnPhotonJoinOrCreateRoomCompleted, bool, bSucess, EPhotonErrorCode, ErrorCode, const FString&, ErrorMessage, int32, InPlayerNumber, const FString&, InPlayerName, bool, bInIsLocalPlayer);

UCLASS()
class SPELLS_API USPhotonConnectAsyncCall : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FSOnPhotonConnectCompleted OnCompleted;
	
	/** Creates a connection async task */
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject", Category = "Spells|Photon Cloud"))
	static USPhotonConnectAsyncCall* PhotonConnect(UObject* WorldContextObject)
	{
		return NewObject<USPhotonConnectAsyncCall>(WorldContextObject);
	}

protected:
	// UBlueprintAsyncActionBase interface begin
	virtual void Activate() override;
	virtual void SetReadyToDestroy() override;
	//~UBlueprintAsyncActionBase interface end

	UFUNCTION()
	void Connected()
	{
		OnCompleted.Broadcast(true, EPhotonErrorCode::OK, TEXT(""));
		SetReadyToDestroy();
	}

	UFUNCTION()
	void ErrorConnecting(EPhotonErrorCode InErrorCode, const FString& InErrorMessage)
	{
		OnCompleted.Broadcast(false, InErrorCode, InErrorMessage);
		SetReadyToDestroy();
	}
};

UCLASS()
class SPELLS_API USPhotonJoinOrCreateRoomAsyncCall : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	/** Creates a room async task */
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject", Category = "Spells|Photon Cloud"))
	static USPhotonJoinOrCreateRoomAsyncCall* PhotonCreateRoom(UObject* WorldContextObject, const FString& InRoomName, const uint8 InMaxPlayers, UPhotonJSON* InInitialProperties)
	{
		USPhotonJoinOrCreateRoomAsyncCall* CreateRoomAsyncCall = NewObject<USPhotonJoinOrCreateRoomAsyncCall>(WorldContextObject);
		CreateRoomAsyncCall->RoomName = InRoomName;
		CreateRoomAsyncCall->MaxPlayers = InMaxPlayers;
		CreateRoomAsyncCall->InitialProperties = InInitialProperties;

		return CreateRoomAsyncCall;
	}

	/** Creates or joins a room async task */
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject", Category = "Spells|Photon Cloud"))
	static USPhotonJoinOrCreateRoomAsyncCall* PhotonJoinOrCreateRoom(UObject* WorldContextObject, const FString& InRoomName, const uint8 InMaxPlayers, UPhotonJSON* InInitialProperties)
	{
		USPhotonJoinOrCreateRoomAsyncCall* JoinOrCreateRoomAsyncCall = NewObject<USPhotonJoinOrCreateRoomAsyncCall>(WorldContextObject);
		JoinOrCreateRoomAsyncCall->RoomName = InRoomName;
		JoinOrCreateRoomAsyncCall->MaxPlayers = InMaxPlayers;
		JoinOrCreateRoomAsyncCall->InitialProperties = InInitialProperties;
		JoinOrCreateRoomAsyncCall->bOnlyCreate = false;

		return JoinOrCreateRoomAsyncCall;
	}

	// UBlueprintAsyncActionBase interface begins
	virtual void Activate() override;
	virtual void SetReadyToDestroy() override;
	// UBlueprintAsyncActionBase interface ends

	UPROPERTY(BlueprintAssignable)
	FSOnPhotonJoinOrCreateRoomCompleted OnCompleted;

protected:
	UFUNCTION()
	void CreateRoomSuccess(int32 InPlayerNumber, FString InPlayerName, bool bInIsLocalPlayer)
	{
		OnCompleted.Broadcast(true, EPhotonErrorCode::OK, TEXT(""), InPlayerNumber, InPlayerName, bInIsLocalPlayer);
		SetReadyToDestroy();
	}

	UFUNCTION()
	void CreateRoomError(EPhotonErrorCode InErrorCode, const FString& InErrorMessage)
	{
		OnCompleted.Broadcast(false, InErrorCode, InErrorMessage, -1, TEXT(""), false);
		SetReadyToDestroy();
	}

private:
	UPROPERTY(Transient)
	UPhotonJSON* InitialProperties = nullptr;

	FString RoomName = TEXT("");
	uint8 MaxPlayers = 2;
	bool bOnlyCreate = true;
};

UCLASS()
class SPELLS_API USPhotonJoinRandomRoomAsyncCall : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	/** Creates a room async task */
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject", Category = "Spells|Photon Cloud"))
	static USPhotonJoinRandomRoomAsyncCall* PhotonJoinRandomRoom(UObject* WorldContextObject, const FString& InLobbyName, ELobbyType InSelectedLobbyType, EMatchMakingMode InSelectedMatchMakingModeconst, uint8 InMaxPlayers, UPhotonJSON* InInitialProperties, const FString& InLobbySQL = "")
	{
		USPhotonJoinRandomRoomAsyncCall* JoinRandomRoomAsyncCall = NewObject<USPhotonJoinRandomRoomAsyncCall>(WorldContextObject);
		JoinRandomRoomAsyncCall->LobbyName = InLobbyName;
		JoinRandomRoomAsyncCall->LobbyType = InSelectedLobbyType;
		JoinRandomRoomAsyncCall->LobbySQL = InLobbySQL;
		JoinRandomRoomAsyncCall->MatchMakingMode = InSelectedMatchMakingModeconst;
		JoinRandomRoomAsyncCall->MaxPlayers = InMaxPlayers;
		JoinRandomRoomAsyncCall->InitialProperties = InInitialProperties;

		return JoinRandomRoomAsyncCall;
	}

	// UBlueprintAsyncActionBase interface begins
	virtual void Activate() override;
	virtual void SetReadyToDestroy() override;
	// UBlueprintAsyncActionBase interface ends
	UPROPERTY(BlueprintAssignable)
	FSOnPhotonJoinOrCreateRoomCompleted OnCompleted;

protected:
	UFUNCTION()
	void JoinedRoomSuccess(int32 InPlayerNumber, FString InPlayerName, bool bInIsLocalPlayer)
	{
		OnCompleted.Broadcast(true, EPhotonErrorCode::OK, TEXT(""), InPlayerNumber, InPlayerName, bInIsLocalPlayer);
		SetReadyToDestroy();
	}

	UFUNCTION()
	void JoinRoomError(EPhotonErrorCode InErrorCode, const FString& InErrorMessage)
	{
		OnCompleted.Broadcast(false, InErrorCode, InErrorMessage, -1, TEXT(""), false);
		SetReadyToDestroy();
	}

private:
	UPROPERTY(Transient)
	UPhotonJSON* InitialProperties = nullptr;

	FString LobbyName = TEXT("");
	ELobbyType LobbyType = ELobbyType::DEFAULT;
	FString LobbySQL = TEXT("");
	EMatchMakingMode MatchMakingMode = EMatchMakingMode::FILL;
	uint8 MaxPlayers = 2;
};