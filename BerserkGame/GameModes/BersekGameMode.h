#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BersekGameMode.generated.h"

UCLASS()
class BERSERKGAME_API ABersekGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ABersekGameMode();

protected:
	virtual void PostLogin(APlayerController* NewPlayer) override;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
		TSubclassOf<APawn> SpectatorPawnClass;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
		TSubclassOf<APawn> PlayerCharacterClass;
};