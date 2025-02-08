#include "PlayerCharacter.h"
#include "BaseMainCharacter.h"
#include "BerserkGame/Characters/MainCharacter/Weapons/MainCharMeleeSword.h"
#include "Camera/CameraShakeBase.h"
#include "Camera/CameraModifier_CameraShake.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/Button.h"
#include "GameFramework/Actor.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AISense_Sight.h"

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	StimuliSourceComponent = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("StimuliSourceComponent"));
	StimuliSourceComponent->RegisterForSense(TSubclassOf<UAISense_Sight>());
	StimuliSourceComponent->bAutoRegister = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = 300.0f;
	SpringArmComp->SocketOffset = FVector(0.0f, 0.0f, 70.0f);
	SpringArmComp->bUsePawnControlRotation = true;

	FollowCameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	FollowCameraComp->bUsePawnControlRotation = true;

	GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);

	BaseTurnRate = 45.0f;
	BaseLookUpRate = 45.0f;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 240.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 600.0f;
	GetCharacterMovement()->AirControl = 0.2f;

	bIsInputEnabled = true;

	//MP
	//static ConstructorHelpers::FClassFinder<ASpectatorPawn> SpectatorPawnBPClass(TEXT("/Game/Blueprints/BP_SpectatorPawn"));
	//if (SpectatorPawnBPClass.Succeeded())
	//{
	//	SpectatorPawnClass = SpectatorPawnBPClass.Class;
	//}

	//static ConstructorHelpers::FClassFinder<APlayerCharacter> PlayerCharacterBPClass(TEXT("/Game/Blueprints/BP_SpectatorPawn"));
	//if (PlayerCharacterBPClass.Succeeded())
	//{
	//	PlayerCharacterClass = PlayerCharacterBPClass.Class;
	//}
}

void APlayerCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	/*check(PlayerInputComponent);*/
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APlayerCharacter::TurnRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &APlayerCharacter::LookUpRate);

	PlayerInputComponent->BindTouch(IE_Pressed, this, &APlayerCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &APlayerCharacter::TouchStopped);

	PlayerInputComponent->BindAction("MeleeAttack", IE_Pressed, this, &ABaseMainCharacter::MeleeAttack);
	PlayerInputComponent->BindAction("RangedAttack", IE_Pressed, this, &ABaseMainCharacter::RangedAttack);

	PlayerInputComponent->BindAction("PauseGame", IE_Pressed, this, &APlayerCharacter::PauseGame);
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (StimuliSourceComponent)
	{
		StimuliSourceComponent->RegisterWithPerceptionSystem();
	}
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

//MP
/*if (!SpectatorPawnClass || !PlayerCharacterClass)
{
	UE_LOG(LogTemp, Warning, TEXT("SpectatorPawnClass or PlayerCharacterClass is not set."));
	return;
}

if (HasAuthority())
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		UE_LOG(LogTemp, Warning, TEXT("Server: Attempting to possess SpectatorPawn"));

		ASpectatorPawn* SpectatorPawn = GetWorld()->SpawnActor<ASpectatorPawn>(SpectatorPawnClass, GetActorLocation(), GetActorRotation());
		if (SpectatorPawn)
		{
			PlayerController->Possess(SpectatorPawn);
			Destroy();
		}
	}
}
else
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, PlayerController]()
			{
				ACharacter* PlayerCharacter = GetWorld()->SpawnActor<ACharacter>(PlayerCharacterClass, GetActorLocation(), GetActorRotation());
				if (PlayerCharacter)
				{
					PlayerController->Possess(PlayerCharacter);
					Destroy();
				}
			}, 1.0f, false);
	}
}
}*/

void APlayerCharacter::StartGame()
{
	if (CurrentWidget)
	{
		CurrentWidget->RemoveFromViewport();
		CurrentWidget = nullptr;
	}

	EnablePlayerInput();
}

void APlayerCharacter::PauseGame()
{
	if (PauseMenuWidgetClass)
	{
		if (CurrentWidget)
		{
			CurrentWidget->RemoveFromViewport();
		}

		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), PauseMenuWidgetClass);
		if (CurrentWidget)
		{
			CurrentWidget->AddToViewport();
			DisablePlayerInput();
			UGameplayStatics::SetGamePaused(GetWorld(), true);
		}
	}
}

void APlayerCharacter::ResumeGame()
{
	if (CurrentWidget)
	{
		CurrentWidget->RemoveFromViewport();
		CurrentWidget = nullptr;
	}

	EnablePlayerInput();
	UGameplayStatics::SetGamePaused(GetWorld(), false);
}

void APlayerCharacter::EnablePlayerInput()
{
	bIsInputEnabled = true;
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PlayerController)
	{
		FInputModeGameOnly InputMode;
		PlayerController->SetInputMode(InputMode);
		PlayerController->bShowMouseCursor = false;
		PlayerController->SetIgnoreMoveInput(false);
		PlayerController->SetIgnoreLookInput(false);
	}
}

void APlayerCharacter::DisablePlayerInput()
{
	bIsInputEnabled = false;
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PlayerController)
	{
		FInputModeUIOnly InputMode;
		PlayerController->SetInputMode(InputMode);
		PlayerController->bShowMouseCursor = true;
		PlayerController->SetIgnoreMoveInput(true);
		PlayerController->SetIgnoreLookInput(true);
	}
}

void APlayerCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Jump();
}

void APlayerCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}

float APlayerCharacter::InflictDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	return 0.0f;
}

void APlayerCharacter::TurnRate(float Rate)
{
	if (IsLocallyControlled())
		UE_LOG(LogTemp, Warning, TEXT("TurnRate = %f"), Rate);

	if (IsLocallyControlled())
		AddControllerYawInput(Rate * DefaultTurnRate);
}

void APlayerCharacter::LookUpRate(float Rate)
{
	AddControllerPitchInput(Rate * DefaultLookUpRate * GetWorld()->GetDeltaSeconds() * 0.5f);
}

void APlayerCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void APlayerCharacter::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}