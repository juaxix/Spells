// Spells - xixgames - juaxix - 2021/2022

#include "Player/SPlayerController.h"

// Unreal includes
#include "Blueprint/UserWidget.h"


void ASPlayerController::ToggleInGameMenu()
{
	if (!IsValid(InGameMenuWidget))
	{
		if (ensureAlwaysMsgf(InGameMenuClass, TEXT("InGameMenuClass is not set for the PlayerController")))
		{
			InGameMenuWidget = CreateWidget<UUserWidget>(this, InGameMenuClass);
		}
	}

	bool bInViewport = InGameMenuWidget->IsInViewport();
	bShowMouseCursor = !bInViewport;

	if (bInViewport)
	{
		InGameMenuWidget->RemoveFromParent();
		SetInputMode(FInputModeGameOnly());
	}
	else
	{
		InGameMenuWidget->AddToViewport(InGameMenuZOrder);
		SetInputMode(FInputModeUIOnly());
	}
}

void ASPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAction("InGameMenu", IE_Pressed, this, &ASPlayerController::ToggleInGameMenu);
}
