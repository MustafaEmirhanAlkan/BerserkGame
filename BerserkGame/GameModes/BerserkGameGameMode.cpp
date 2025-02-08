// Default Class


#include "BerserkGameGameMode.h"
#include "BerserkGame/Characters/BerserkGameCharacter.h"
#include "UObject/ConstructorHelpers.h"

ABerserkGameGameMode::ABerserkGameGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

//DELETE THIS CODE.