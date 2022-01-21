// Spells - xixgames - juaxix - 2021/2022

#include "UI/SWorldUserWidget.h"

// Unreal includes
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/SizeBox.h"
#include "Kismet/GameplayStatics.h"


void USWorldUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (!IsValid(ParentSizeBox))
	{
		return;
	}

	FVector2D ScreenPosition;
	if (UGameplayStatics::ProjectWorldToScreen(GetOwningPlayer(), AttachedActor->GetActorLocation(), ScreenPosition))
	{
		float ViewportScale = UWidgetLayoutLibrary::GetViewportScale(this);
		ScreenPosition /= ViewportScale; //adjust for this widget
		ParentSizeBox->SetRenderTranslation(ScreenPosition);
		ParentSizeBox->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else
	{
		ParentSizeBox->SetVisibility(ESlateVisibility::Collapsed);
	}
}
