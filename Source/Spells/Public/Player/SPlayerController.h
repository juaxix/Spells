// Spells - xixgames - juaxix - 2021/2022

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSOnBeginPlayingState);

/**
 * Base class for the Spells Game Player Controller
 */
UCLASS()
class SPELLS_API ASPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void ToggleInGameMenu();

	/* Called when the player controller is ready to begin playing */
	virtual void BeginPlayingState() override { OnBeginPlayingState.Broadcast(); }

	virtual void SetupInputComponent() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spells|UI")
	TSubclassOf<UUserWidget> InGameMenuClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spells|UI")
	int32 InGameMenuZOrder = 100;

	UPROPERTY(VisibleInstanceOnly, BlueprintAssignable, Category = "Spells|Player Controller")
	FSOnBeginPlayingState OnBeginPlayingState;

	UPROPERTY()
	UUserWidget* InGameMenuWidget = nullptr;
};
