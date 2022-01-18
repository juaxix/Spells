// Spells - xixgames - juaxix - 2021/2022

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SPlayerState.generated.h"

class USSaveGame;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FSOnCreditsChanged, class ASPlayerState*, PlayerState, int32, NewCredits, int32, Delta);

/**
 * Store player information during the game
 */
UCLASS()
class SPELLS_API ASPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Spells|Credits") FORCEINLINE
	int32 GetCredits() const { return Credits; }

	UFUNCTION(BlueprintCallable, Category = "Spells|Credits")
	void GrantCredits(int32 Delta);

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Spells|Credits")
	bool ConsumeCredits(int32 Delta);

	UFUNCTION(BlueprintNativeEvent)
	void SavePlayerState(USSaveGame* SaveGame);

	UFUNCTION(BlueprintNativeEvent)
	void LoadPlayerState(USSaveGame* SaveGame);

	UPROPERTY(BlueprintAssignable, Category = "Spells|Credits")
	FSOnCreditsChanged OnCreditsChanged;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Spells|Credits")
	int32 Credits = 0;
	
};
