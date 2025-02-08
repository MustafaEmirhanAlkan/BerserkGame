#include "BerserkGame/Characters/Spectators/MainMenuSpectator.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

AMainMenuSpectator::AMainMenuSpectator()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AMainMenuSpectator::BeginPlay()
{
	Super::BeginPlay();

	if (MainMenuWidgetClass)
	{
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), MainMenuWidgetClass);
		if (CurrentWidget)
		{
			CurrentWidget->AddToViewport();

			UButton* Button_Play = Cast<UButton>(CurrentWidget->GetWidgetFromName(TEXT("Button_Play")));
			if (Button_Play)
			{
				Button_Play->OnClicked.AddDynamic(this, &AMainMenuSpectator::StartGame);
			}
		}
	}
}

void AMainMenuSpectator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMainMenuSpectator::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AMainMenuSpectator::StartGame()
{
	if (CurrentWidget)
	{
		CurrentWidget->RemoveFromViewport();
		CurrentWidget = nullptr;
	}

	UGameplayStatics::OpenLevel(this, FName("GameMap"));
}