#pragma once

#include "BaseMainCharacter.h"
#include "Blueprint/UserWidget.h"
#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "Components/Button.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/SpectatorPawn.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "PlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;

UCLASS(config = Game)
class APlayerCharacter : public  ABaseMainCharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	float InflictDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);
	
	UPROPERTY(EditAnywhere, Category = "Animations")
		TArray<class UAnimMontage*> Attacks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
		USpringArmComponent* SpringArmComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
		UCameraComponent* FollowCameraComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
		float DefaultTurnRate = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
		float DefaultLookUpRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		TSubclassOf<class UMainCharAnimInstance> MainCharAnimInstanceClass;

	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	//	TSubclassOf<class UUserWidget> MainMenuWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
		TSubclassOf<class UUserWidget> PauseMenuWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
		TSubclassOf<class UUserWidget> RestartMenuWidgetClass;

	UFUNCTION(BlueprintCallable, Category = "UI")
		void StartGame();

	//UFUNCTION(BlueprintCallable, Category = "UI")
	//	void ShowMainMenu();

	UFUNCTION(BlueprintCallable, Category = "UI")
		void PauseGame();

	UFUNCTION(BlueprintCallable, Category = "UI")
		void ResumeGame();

	//MP
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spectator")
	//	TSubclassOf<ASpectatorPawn> SpectatorPawnClass;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spectator")
	//	TSubclassOf<APlayerCharacter> PlayerCharacterClass;
protected:
	virtual void BeginPlay() override;
	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnRate(float Rate);
	void LookUpRate(float Rate);
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
		UAIPerceptionStimuliSourceComponent* StimuliSourceComponent;

	UUserWidget* CurrentWidget;
	bool bIsInputEnabled;

	void EnablePlayerInput();
	void DisablePlayerInput();
};