// Spells - xixgames - juaxix - 2021/2022

#pragma once

// Unreal includes
#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

#include "SGameInstance.generated.h"

/**
 * Game instance , to set the Photon Cloud Object instance on Initialize
 */
UCLASS()
class SPELLS_API USGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;

	virtual void OnWorldChanged(UWorld* OldWorld, UWorld* NewWorld) override;
};
