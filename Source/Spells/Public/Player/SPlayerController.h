// Spells - xixgames - juaxix - 2021/2022

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SPlayerController.generated.h"

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

	virtual void SetupInputComponent() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spells|UI")
	TSubclassOf<UUserWidget> InGameMenuClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spells|UI")
	int32 InGameMenuZOrder = 100;

	UPROPERTY()
	UUserWidget* InGameMenuWidget = nullptr;
};
