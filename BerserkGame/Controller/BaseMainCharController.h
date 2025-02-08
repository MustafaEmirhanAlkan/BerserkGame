#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BerserkGame/Characters/MainCharacter/BaseMainCharacter.h"
#include "BaseMainCharController.generated.h"

UCLASS()
class BERSERKGAME_API ABaseMainCharController : public APlayerController
{
	GENERATED_BODY()

public:
	ABaseMainCharController();
	//Spectator
	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);

	float BaseTurnRate;
	float BaseLookUpRate;

protected:
	virtual void SetupInputComponent() override;

	void MeleeAttack();
	void RangedAttack();
};