#include "BersekGameMode.h"
#include "BerserkGame/Characters/MainCharacter/PlayerCharacter.h"
#include "BerserkGame/Controller/BaseMainCharController.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/SpectatorPawn.h"

ABersekGameMode::ABersekGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprint/Characters/MainCharacter/BP_PlayerCharacter"));
	if (PlayerPawnBPClass.Succeeded())
	{
		PlayerCharacterClass = PlayerPawnBPClass.Class;
	}

	static ConstructorHelpers::FClassFinder<APawn> SpectatorPawnBPClass(TEXT("/Game/Blueprint/Characters/MainCharacter/BP_SpectatorPawn"));
	if (SpectatorPawnBPClass.Succeeded())
	{
		SpectatorPawnClass = SpectatorPawnBPClass.Class;
	}

	DefaultPawnClass = PlayerCharacterClass;
	PlayerControllerClass = ABaseMainCharController::StaticClass();
}

void ABersekGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (HasAuthority())
	{
		if (GetNumPlayers() == 1)
		{
			if (NewPlayer->GetPawn())
			{
				NewPlayer->GetPawn()->Destroy();
			}

			APlayerStart* PlayerStart = Cast<APlayerStart>(FindPlayerStart(NewPlayer));
			if (PlayerStart)
			{
				FTransform SpectatorSpawnTransform = PlayerStart->GetActorTransform();

				ASpectatorPawn* SpectatorPawn = GetWorld()->SpawnActor<ASpectatorPawn>(SpectatorPawnClass, SpectatorSpawnTransform.GetLocation(), SpectatorSpawnTransform.GetRotation().Rotator());
				if (SpectatorPawn)
				{
					NewPlayer->Possess(SpectatorPawn);

					NewPlayer->SetIgnoreMoveInput(false);
					NewPlayer->SetIgnoreLookInput(false);
					NewPlayer->bShowMouseCursor = true;
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("No PlayerStart found for SpectatorPawn!"));
			}
		}
		else
		{
			APlayerStart* PlayerStart = Cast<APlayerStart>(FindPlayerStart(NewPlayer));
			if (PlayerStart)
			{
				FTransform SpawnTransform = PlayerStart->GetActorTransform();
				if (NewPlayer->GetPawn())
				{
					NewPlayer->GetPawn()->Destroy();
				}

				APawn* PlayerCharacter = GetWorld()->SpawnActor<APawn>(PlayerCharacterClass, SpawnTransform);
				if (PlayerCharacter)
				{
					NewPlayer->Possess(PlayerCharacter);
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("No PlayerStart found for PlayerCharacter!"));
			}
		}
	}
}