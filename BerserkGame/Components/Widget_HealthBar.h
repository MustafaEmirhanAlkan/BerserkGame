#pragma once

#include "Blueprint/UserWidget.h"
#include "CoreMinimal.h"
#include "Components/ProgressBar.h"
#include "Widget_HealthBar.generated.h"

UCLASS()
class BERSERKGAME_API UWidget_HealthBar : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UWidget_HealthBar(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;
	void SetBarValuePercent(float const value);

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
		UProgressBar* HealthValue = nullptr;

private:
	UPROPERTY(meta = (BindWidget))
		class UProgressBar* HealthProgressBar;
};
