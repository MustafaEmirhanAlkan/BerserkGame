#include "BerserkGame/Components/Widget_HealthBar.h"
#include "Components/ProgressBar.h"

UWidget_HealthBar::UWidget_HealthBar(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UWidget_HealthBar::NativeConstruct()
{
	Super::NativeConstruct();

	HealthValue = Cast<UProgressBar>(GetWidgetFromName(TEXT("HealthValue")));
	if (HealthValue == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("HealthValue is nullptr! Make sure it's properly set in the widget blueprint."));
	}
}

void UWidget_HealthBar::SetBarValuePercent(float const value)
{
	if (HealthValue)
	{
		HealthValue->SetPercent(value);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("HealthValue is nullptr!"));
	}
}