#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpectatorPawn.h"
#include "MainMenuSpectator.generated.h"

UCLASS()
class BERSERKGAME_API AMainMenuSpectator : public ASpectatorPawn
{
	GENERATED_BODY()

public:
	AMainMenuSpectator();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
		void StartGame();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
		TSubclassOf<class UUserWidget> MainMenuWidgetClass;

private:
	UPROPERTY()
		UUserWidget* CurrentWidget;


};
