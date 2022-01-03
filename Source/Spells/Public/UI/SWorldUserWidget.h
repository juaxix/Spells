// Spells - xixgames - juaxix - 2021/2022

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SWorldUserWidget.generated.h"

class USizeBox;

/**
 * Anything needed to be projected from the World to the HUD
 */
UCLASS()
class SPELLS_API USWorldUserWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Target")
	AActor* AttachedActor = nullptr;

protected:
	UPROPERTY(EditAnywhere, meta = (BindWidget)) //This property requires a widget be bound to it in the designer.  Allows easy native access to designer defined controls.
	USizeBox* ParentSizeBox;
	
};
