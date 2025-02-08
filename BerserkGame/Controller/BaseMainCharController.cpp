#include "BerserkGame/Controller/BaseMainCharController.h"
#include "BerserkGame/GameModes/BerserkGame.h"
#include "BerserkGame/Characters/MainCharacter/BaseMainCharacter.h"
#include "GameFramework/SpectatorPawn.h"
#include "GameFramework/SpectatorPawnMovement.h"

ABaseMainCharController::ABaseMainCharController()
{
	bShowMouseCursor = false;
}

void ABaseMainCharController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("MeleeAttack", IE_Pressed, this, &ABaseMainCharController::MeleeAttack);
	InputComponent->BindAction("RangedAttack", IE_Pressed, this, &ABaseMainCharController::RangedAttack);
	//Spectator
	InputComponent->BindAxis("MoveForward", this, &ABaseMainCharController::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ABaseMainCharController::MoveRight);
	InputComponent->BindAxis("TurnRate", this, &ABaseMainCharController::TurnAtRate);
	InputComponent->BindAxis("LookUpRate", this, &ABaseMainCharController::LookUpAtRate);
}

void ABaseMainCharController::MeleeAttack()
{
	ABaseMainCharacter* BaseMainCharacter = Cast<ABaseMainCharacter>(GetPawn());
	if (BaseMainCharacter)
	{
		BaseMainCharacter->MeleeAttack();
	}
}

void ABaseMainCharController::RangedAttack()
{
	ABaseMainCharacter* BaseMainCharacter = Cast<ABaseMainCharacter>(GetPawn());
	if (BaseMainCharacter)
	{
		BaseMainCharacter->RangedAttack();
	}
}
//Spectator
void ABaseMainCharController::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		APawn* ControlledPawn = GetPawn();
		if (ControlledPawn)
		{
			ControlledPawn->AddMovementInput(ControlledPawn->GetActorForwardVector(), Value);
		}
	}
}

void ABaseMainCharController::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		APawn* ControlledPawn = GetPawn();
		if (ControlledPawn)
		{
			ControlledPawn->AddMovementInput(ControlledPawn->GetActorRightVector(), Value);
		}
	}
}

void ABaseMainCharController::TurnAtRate(float Rate)
{
	AddYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ABaseMainCharController::LookUpAtRate(float Rate)
{
	AddPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}