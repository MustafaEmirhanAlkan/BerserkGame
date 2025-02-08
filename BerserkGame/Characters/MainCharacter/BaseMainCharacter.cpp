#include "BaseMainCharacter.h"
#include "BerserkGame/Components/AC_HealthComponent.h"
#include "BerserkGame/Components/Widget_HealthBar.h"
#include "BerserkGame/Characters/MainCharacter/Weapons/MainCharRangedProjectile.h"
#include "BerserkGame/Characters/MainCharacter/Weapons/MainCharMeleeSword.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "MainCharAnimInstance.h"
#include "Net/UnrealNetwork.h" //MP
#include "Runtime/Engine/Classes/Components/BoxComponent.h"
#include "Sound/SoundBase.h"
#include "../EnemyCharacters/BaseAIEnemy.h"
#include "AIController.h"

ABaseMainCharacter::ABaseMainCharacter() : Widget_Component(CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthValue")))
{
	PrimaryActorTick.bCanEverTick = true;

	Attacking = false;
	AttackDamaging = false;
	LastRotationSpeed = 0.60f;
	BaseTurnRate = 30.f;
	BaseLookUpRate = 30.f;
	TurnRate = 30.f;
	LookUpRate = 30.f;
	MeleeDamage = 25.0f;
	FireballDamage = 35.0f;

	HealthComponent = CreateDefaultSubobject<UAC_HealthComponent>(TEXT("HealthComponent"));

	HealthBarWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidgetComponent"));
	HealthBarWidgetComponent->SetupAttachment(RootComponent);
	HealthBarWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	HealthBarWidgetComponent->SetDrawSize(FVector2D(200.0f, 50.0f));
	static ConstructorHelpers::FClassFinder<UUserWidget> HealthBarWidgetClassFinder(TEXT("/Game/Blueprint/Widgets/Widget_HealthBar"));
	if (HealthBarWidgetClassFinder.Succeeded())
	{
		HealthBarWidgetComponent->SetWidgetClass(HealthBarWidgetClassFinder.Class);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to find Widget_HealthBar class at /Game/Blueprints/Widgets/Widget_HealthBar"));
	}

	static ConstructorHelpers::FClassFinder<AMainCharMeleeSword> SwordClassFinder(TEXT("/Game/Blueprint/Weapons/BP_Sword"));
	if (SwordClassFinder.Succeeded())
	{
		SwordClass = SwordClassFinder.Class;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to find sword class at /Game/Blueprint/Weapons/BP_Sword"));
	}

	MeleeAttackCooldownTime = 1.0f;
	RangedAttackCooldownTime = 2.0f;
	LastMeleeAttackTime = -MeleeAttackCooldownTime;
	LastRangedAttackTime = -RangedAttackCooldownTime;

	//LastMeleeAttackTime = 0.0f;
	//LastRangedAttackTime = 0.0f;

	AttackEffortSound = nullptr;
	SwordSwingSound = nullptr;
	ProjectileSpawnSound = nullptr;
	PainSound = nullptr;
	DeathSound = nullptr;

	//MP
	bReplicates = true;
}

//MP
void ABaseMainCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABaseMainCharacter, LastMeleeAttackTime);
	DOREPLIFETIME(ABaseMainCharacter, LastRangedAttackTime);
	DOREPLIFETIME(ABaseMainCharacter, HealthComponent);
}

UAnimMontage* ABaseMainCharacter::Get_montage() const
{
	return montage;
}

void ABaseMainCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (!SwordClass)
	{
		UE_LOG(LogTemp, Error, TEXT("SwordClass is not set. Please set SwordClass in the Blueprint or Constructor."));
		return;
	}

	AttachSwordToCharacter();

	MainCharAnimInstance = Cast<UMainCharAnimInstance>(GetMesh()->GetAnimInstance());
	if (!MainCharAnimInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to cast AnimInstance"));
	}

	//UE_LOG(LogTemp, Warning, TEXT("An Actor's name is %s"), *MainCharAnimInstance->GetFName().ToString());

	if (HealthComponent)
	{
		HealthComponent->OnHealthChanged.AddDynamic(this, &ABaseMainCharacter::OnHealthChanged);
	}

	if (HealthBarWidgetComponent)
	{
		HealthBarWidgetComponent->InitWidget();
		UUserWidget* HealthBarWidget = HealthBarWidgetComponent->GetUserWidgetObject();
		if (HealthBarWidget)
		{
			HealthBarWidgetInstance = Cast<UWidget_HealthBar>(HealthBarWidget);
			if (HealthBarWidgetInstance)
			{
				HealthBarWidgetInstance->AddToViewport();
				HealthBarWidgetInstance->SetAnchorsInViewport(FAnchors(0, 1, 0, 1));
				HealthBarWidgetInstance->SetAlignmentInViewport(FVector2D(0, 1));

				FVector2D OffsetFromViewportBottomLeft(20.0f, -20.0f);
				HealthBarWidgetInstance->SetPositionInViewport(OffsetFromViewportBottomLeft, true);
			}
		}
	}
}

void ABaseMainCharacter::AttachSwordToCharacter()
{
	if (SwordClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();

		FTransform SocketTransform = GetMesh()->GetSocketTransform(FName("MeleeWeaponSocket"));
		EquippedSword = GetWorld()->SpawnActor<AMainCharMeleeSword>(SwordClass, SocketTransform, SpawnParams);
		EquippedSword->SetOwnerChar(this);
		MeleeWeapon = EquippedSword;

		if (EquippedSword)
		{
			EquippedSword->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("MeleeWeaponSocket"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to spawn sword."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("SwordClass is not valid."));
	}
}

void ABaseMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const FVector& Velocity = GetVelocity();
	const float Speed = Velocity.Size();

	//MP
	if (GEngine)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("LastMeleeAttackTime: %f"), LastMeleeAttackTime));
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("LastRangedAttackTime: %f"), LastRangedAttackTime));
	}
}

void ABaseMainCharacter::Attack()
{
	Attacking = true;
	AttackDamaging = false;
}

void ABaseMainCharacter::AttackLunge()
{
	Attacking = true;
}

void ABaseMainCharacter::EndAttack()
{
	Attacking = false;
}

void ABaseMainCharacter::SetAttackDamaging(bool Damaging)
{
	AttackDamaging = Damaging;
}

void ABaseMainCharacter::MeleeAttack()
{

	if (GetWorld()->TimeSeconds - LastMeleeAttackTime < MeleeAttackCooldownTime)
	{
		UE_LOG(LogTemp, Warning, TEXT("Melee attack is on cooldown! Time remaining: %f seconds"),
			MeleeAttackCooldownTime - (GetWorld()->TimeSeconds - LastMeleeAttackTime));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("MeleeAttack!!!!!"));

	if (IsLocallyControlled())
	{
		LastMeleeAttackTime = GetWorld()->GetTimeSeconds();
		MainCharAnimInstance->PlaySword_And_Shield_Slash();
		MeleeAttack_Internal();
		ServerMeleeAttack();
	}
	else
	{
		ServerMeleeAttack();
	}

	if (AttackEffortSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, AttackEffortSound, GetActorLocation());
	}
	if (SwordSwingSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, SwordSwingSound, GetActorLocation());
	}

	FVector Start = GetMesh()->GetSocketLocation(TEXT("MeleeWeaponSocket"));
	FVector ForwardVector = GetActorForwardVector();
	FVector End = Start + ForwardVector * 50.0f;

	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	if (GetWorld())
	{
		DrawDebugLine(GetWorld(), Start, End, FColor::Blue, false, 2.0f, 0, 1.0f);
	}
	LastMeleeAttackTime = GetWorld()->TimeSeconds;
	UE_LOG(LogTemp, Log, TEXT("Last melee attack time: %f"), LastMeleeAttackTime);
}

//MP
void ABaseMainCharacter::MeleeAttack_Internal()
{
	if (IsLocallyControlled())
	{
		if (GetWorld()->TimeSeconds - LastMeleeAttackTime < MeleeAttackCooldownTime)
		{
			UE_LOG(LogTemp, Warning, TEXT("Melee attack is on cooldown! Time remaining: %f seconds"),
				MeleeAttackCooldownTime - (GetWorld()->TimeSeconds - LastMeleeAttackTime));
			return;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("MeleeAttack!!!!!"));

	if (AttackEffortSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, AttackEffortSound, GetActorLocation());
	}
	if (SwordSwingSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, SwordSwingSound, GetActorLocation());
	}

	FVector Start = GetMesh()->GetSocketLocation(TEXT("MeleeWeaponSocket"));
	FVector ForwardVector = GetActorForwardVector();
	FVector End = Start + ForwardVector * 50.0f;

	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);
	MainCharAnimInstance->PlaySword_And_Shield_Slash();

	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams))
	{
		AActor* HitActor = HitResult.GetActor();
		if (HitActor)
		{
			if (ActorHasTag("Player") && HitActor->ActorHasTag("Enemy"))
			{
				InflictDamage(MeleeDamage, FDamageEvent(), nullptr, this);
			}
			else if (ActorHasTag("Enemy") && HitActor->ActorHasTag("Player"))
			{
				InflictDamage(MeleeDamage, FDamageEvent(), nullptr, this);
			}
		}
	}

	if (GetWorld())
	{
		DrawDebugLine(GetWorld(), Start, End, FColor::Blue, false, 2.0f, 0, 1.0f);
	}
	LastMeleeAttackTime = GetWorld()->TimeSeconds;
	UE_LOG(LogTemp, Log, TEXT("Last melee attack time: %f"), LastMeleeAttackTime);
}

void ABaseMainCharacter::ServerMeleeAttack_Implementation()
{
	MeleeAttack_Internal();
	MLC_MeleeAttack();
	MulticastPlayMeleeAttackAnimation();
}

bool ABaseMainCharacter::ServerMeleeAttack_Validate()
{
	return true;
}

void ABaseMainCharacter::MulticastPlayMeleeAttackAnimation_Implementation()
{
	MainCharAnimInstance->PlaySword_And_Shield_Slash();
}

void ABaseMainCharacter::OnRep_LastMeleeAttackTime()
{
	if (GetWorld()->TimeSeconds - LastMeleeAttackTime < MeleeAttackCooldownTime)
	{
		UE_LOG(LogTemp, Warning, TEXT("Melee attack is on cooldown! Time remaining: %f seconds"),
			MeleeAttackCooldownTime - (GetWorld()->TimeSeconds - LastMeleeAttackTime));
		return;
	}
}

void ABaseMainCharacter::MLC_MeleeAttack_Implementation()
{
	if (IsLocallyControlled())
		return;

	MeleeAttack_Internal();

}

void ABaseMainCharacter::RangedAttack()
{
	if (HasAuthority())
	{
		ServerRangedAttack();
		return;
	}

	if (GetWorld()->TimeSeconds - LastRangedAttackTime < RangedAttackCooldownTime)
	{
		UE_LOG(LogTemp, Warning, TEXT("Ranged attack is on cooldown! Time remaining: %f seconds"),
			RangedAttackCooldownTime - (GetWorld()->TimeSeconds - LastRangedAttackTime));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("RangedAttack!!!!!"));

	if (HasAuthority())
	{
		LastRangedAttackTime = GetWorld()->GetTimeSeconds();
		RangedAttack_Internal();
		MLC_RangedAttack();
	}
	else
	{
		ServerRangedAttack();
	}

	FVector Start = GetMesh()->GetSocketLocation(TEXT("head"));
	FVector ForwardVector = GetActorForwardVector();
	FVector SpawnLocation = Start + (ForwardVector * 100.0f);
	FVector End = Start + ForwardVector * 3000.0f;
	FRotator SpawnRotation = ForwardVector.Rotation();

	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	MainCharAnimInstance->PlayStanding_1H_Magic_Attack_01();

	bool bHasDetectedEnemy = false;
	AActor* DetectedEnemy = nullptr;
	float DetectionRadius = 500.0f;

	TArray<FOverlapResult> OverlapResults;
	FCollisionShape DetectionSphere = FCollisionShape::MakeSphere(DetectionRadius);

	if (GetWorld()->OverlapMultiByChannel(OverlapResults, SpawnLocation, FQuat::Identity, ECC_Pawn, DetectionSphere, CollisionParams))
	{
		for (const FOverlapResult& Overlap : OverlapResults)
		{
			if (AActor* OverlappedActor = Overlap.GetActor())
			{
				if (ActorHasTag("Player") && OverlappedActor->ActorHasTag("Enemy"))
				{
					DetectedEnemy = OverlappedActor;
					bHasDetectedEnemy = true;
					break;
				}
				else if (ActorHasTag("Enemy") && OverlappedActor->ActorHasTag("Player"))
				{
					DetectedEnemy = OverlappedActor;
					bHasDetectedEnemy = true;
					break;
				}
			}
		}
	}

	if (GetWorld())
	{
		DrawDebugLine(GetWorld(), SpawnLocation, End, FColor::Red, false, 2.0f, 0, 1.0f);
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AMainCharRangedProjectile* Projectile = GetWorld()->SpawnActor<AMainCharRangedProjectile>(ProjectileInstance, SpawnLocation, SpawnRotation, SpawnParams);

	if (Projectile)
	{
		Projectile->SetDamage(FireballDamage);
		Projectile->SetLifeSpan(3.0f);
		Projectile->IgnoreOwner();

		if (bHasDetectedEnemy)
		{
			Projectile->HomingTowards(DetectedEnemy);
		}
	}

	//LastRangedAttackTime = GetWorld()->TimeSeconds;
	//UE_LOG(LogTemp, Log, TEXT("Last ranged attack time: %f"), LastRangedAttackTime);

	//RangedAttack_Internal();
	//MLC_RangedAttack();
}

//MP
void ABaseMainCharacter::RangedAttack_Internal()
{

	if (GetWorld()->TimeSeconds - LastRangedAttackTime < RangedAttackCooldownTime)
	{
		UE_LOG(LogTemp, Warning, TEXT("Ranged attack is on cooldown! Time remaining: %f seconds"),
			RangedAttackCooldownTime - (GetWorld()->TimeSeconds - LastRangedAttackTime));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("RangedAttack!!!!!"));

	if (ProjectileSpawnSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ProjectileSpawnSound, GetActorLocation());
	}

	FVector Start = GetMesh()->GetSocketLocation(TEXT("head"));
	FVector ForwardVector = GetActorForwardVector();
	FVector SpawnLocation = Start + (ForwardVector * 100.0f);
	FVector End = Start + ForwardVector * 3000.0f;
	FRotator SpawnRotation = ForwardVector.Rotation();

	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	MainCharAnimInstance->PlayStanding_1H_Magic_Attack_01();

	bool bHasDetectedEnemy = false;
	AActor* DetectedEnemy = nullptr;
	float DetectionRadius = 500.0f;

	TArray<FOverlapResult> OverlapResults;
	FCollisionShape DetectionSphere = FCollisionShape::MakeSphere(DetectionRadius);

	if (GetWorld()->OverlapMultiByChannel(OverlapResults, SpawnLocation, FQuat::Identity, ECC_Pawn, DetectionSphere, CollisionParams))
	{
		for (const FOverlapResult& Overlap : OverlapResults)
		{
			if (AActor* OverlappedActor = Overlap.GetActor())
			{
				if (ActorHasTag("Player") && OverlappedActor->ActorHasTag("Enemy"))
				{
					DetectedEnemy = OverlappedActor;
					bHasDetectedEnemy = true;
					break;
				}
				else if (ActorHasTag("Enemy") && OverlappedActor->ActorHasTag("Player"))
				{
					DetectedEnemy = OverlappedActor;
					bHasDetectedEnemy = true;
					break;
				}
			}
		}
	}

	if (GetWorld())
	{
		DrawDebugLine(GetWorld(), SpawnLocation, End, FColor::Red, false, 2.0f, 0, 1.0f);
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	//AMainCharRangedProjectile* Projectile = GetWorld()->SpawnActor<AMainCharRangedProjectile>(ProjectileInstance, SpawnLocation, SpawnRotation, SpawnParams);
	if (HasAuthority())
	{
		AMainCharRangedProjectile* Projectile = GetWorld()->SpawnActor<AMainCharRangedProjectile>(ProjectileInstance, SpawnLocation, SpawnRotation, SpawnParams);

		if (Projectile)
		{
			Projectile->SetDamage(FireballDamage);
			Projectile->SetLifeSpan(3.0f);
			Projectile->IgnoreOwner();

			if (bHasDetectedEnemy)
			{
				Projectile->HomingTowards(DetectedEnemy);
			}
		}
	}
	LastRangedAttackTime = GetWorld()->TimeSeconds;
	UE_LOG(LogTemp, Log, TEXT("Last ranged attack time: %f"), LastRangedAttackTime);
}

void ABaseMainCharacter::ServerRangedAttack_Implementation()
{
	//RangedAttack_Internal();
	/*LastRangedAttackTime = GetWorld()->GetTimeSeconds();*/
	MLC_RangedAttack();
	MulticastPlayRangedAttackAnimation();
	//RangedAttack();
}

bool ABaseMainCharacter::ServerRangedAttack_Validate()
{
	return true;
}

void ABaseMainCharacter::MulticastPlayRangedAttackAnimation_Implementation()
{
	MainCharAnimInstance->PlayStanding_1H_Magic_Attack_01();
}

void ABaseMainCharacter::OnRep_LastRangedAttackTime()
{
	if (GetWorld()->TimeSeconds - LastRangedAttackTime < RangedAttackCooldownTime)
	{
		UE_LOG(LogTemp, Warning, TEXT("Ranged attack is on cooldown! Time remaining: %f seconds"),
			RangedAttackCooldownTime - (GetWorld()->TimeSeconds - LastRangedAttackTime));
		return;
	}

	MLC_RangedAttack();
}

void ABaseMainCharacter::MLC_RangedAttack_Implementation()
{
	if (IsLocallyControlled())
		return;

	RangedAttack_Internal();

}

void ABaseMainCharacter::On_MeleeEnemyAttack()
{
	if (MeleeMontage)
	{
		PlayAnimMontage(MeleeMontage);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Faild"));
	}
}

void ABaseMainCharacter::On_RangedEnemyAttack()
{
	if (RangedMontage)
	{
		PlayAnimMontage(RangedMontage);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Faild"));
	}
}

void ABaseMainCharacter::MulticastPlayHitReactionAnimation_Implementation()
{
	if (MainCharAnimInstance && MainCharAnimInstance->HitReactionMontage)
	{
		MainCharAnimInstance->Montage_Play(MainCharAnimInstance->HitReactionMontage);
	}
}

void ABaseMainCharacter::MulticastPlayHitReactionSound_Implementation()
{
	if (PainSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, PainSound, GetActorLocation());
	}
}

void ABaseMainCharacter::MulticastPlayDeathAnimationAndSound_Implementation()
{
	if (MainCharAnimInstance && MainCharAnimInstance->DeathAnimationMontage)
	{
		MainCharAnimInstance->Montage_Play(MainCharAnimInstance->DeathAnimationMontage);
	}

	if (DeathSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());
	}
}

float ABaseMainCharacter::Get_Health() const
{
	return HealthComponent->Health;
}

float ABaseMainCharacter::Get_Max_Health() const
{
	return HealthComponent->MaxHealth;
}

bool ABaseMainCharacter::GetIsDeath() const
{
	return HealthComponent->Health <= 0;
}

void ABaseMainCharacter::Set_Health(float const NewHealth)
{
	bool IsDeath = GetIsDeath();

	if (IsDeath)
	{
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

		if (Cast<ABaseMainCharacter>(this))
		{
			if (NewHealth <= 0)
			{
				UE_LOG(LogTemp, Error, TEXT("DIED."));
				GetWorld()->GetFirstPlayerController()->ConsoleCommand("Restart.");
				//Destroy(); //Destroys main char.
			}
		}
		else if (Cast<ABaseAIEnemy>(this))
		{
			if (NewHealth <= 0)
			{
				UE_LOG(LogTemp, Error, TEXT("Enemy Dead."));
				GetWorld()->GetFirstPlayerController()->ConsoleCommand("Restart.");
				//Destroy();
			}
		}

		auto const User_Widget = Cast<UWidget_HealthBar>(Widget_Component->GetUserWidgetObject());
		if (User_Widget)
		{
			//User_Widget->SetBarValuePercent(Health / Max_Health);
		}
		UE_LOG(LogTemp, Warning, TEXT("An Actor's health is %f"), NewHealth);

	}
}

void ABaseMainCharacter::ServerInflictDamage_Implementation(float DamageAmount, AActor* DamageCauser)
{
	if (HasAuthority())
	{
		float CalculatedDamage = FMath::Min(HealthComponent->Health, DamageAmount);
		HealthComponent->Health -= CalculatedDamage;

		if (HealthComponent->Health <= 0)
		{
			MulticastPlayDeathAnimationAndSound();
		}
		else
		{
			MulticastPlayHitReactionAnimation();
			MulticastPlayHitReactionSound();
		}
		OnHealthChanged(HealthComponent->Health, HealthComponent->MaxHealth);
	}
}

bool ABaseMainCharacter::ServerInflictDamage_Validate(float DamageAmount, AActor* DamageCauser)
{
	return true;
}


float ABaseMainCharacter::InflictDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{

	if (!HasAuthority())
	{
		ServerInflictDamage(DamageAmount, DamageCauser);
		return 0;
	}

	if (DamageCauser == this /*&& DamageCauser ==*/ )
	{
		return 0;
	}

	if (HealthComponent->Health <= 0)
	{
		return 0;
	}

	UE_LOG(LogTemp, Warning, TEXT("InflictDamage called on %s by %s"), *GetNameSafe(this), *GetNameSafe(DamageCauser));

	float DamageCaused = FMath::Min(HealthComponent->Health, DamageAmount);
	HealthComponent->Health -= DamageCaused;

	MulticastPlayHitReactionAnimation();

	if (HealthComponent->Health <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Character Died.....!"));
		DisableInput(GetWorld()->GetFirstPlayerController());
		GetCharacterMovement()->DisableMovement();
		GetMesh()->SetSimulatePhysics(false);

		UMainCharAnimInstance* AnimInstance = Cast<UMainCharAnimInstance>(GetMesh()->GetAnimInstance());
		if (AnimInstance && AnimInstance->DeathAnimationMontage)
		{
			AnimInstance->Montage_Play(AnimInstance->DeathAnimationMontage);
		}
		PlayDamageSounds(true);
		DisableInput(GetWorld()->GetFirstPlayerController());

		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

		AAIController* AIController = Cast<AAIController>(GetController());
		if (AIController)
		{
			AIController->BrainComponent->StopLogic(TEXT("Character Died"));
		}

		if (HealthComponent)
		{
			HealthComponent->SetComponentTickEnabled(false);
		}

		if (Widget_Component)
		{
			Widget_Component->SetVisibility(false);
		}

		if (RestartWidget)
		{
			RestartWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), RestartWidget);
			if (RestartWidgetInstance)
			{
				RestartWidgetInstance->AddToViewport();

				APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
				if (PlayerController)
				{
					FInputModeUIOnly InputModeData;
					InputModeData.SetWidgetToFocus(RestartWidgetInstance->TakeWidget());
					InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

					PlayerController->SetInputMode(InputModeData);
					PlayerController->bShowMouseCursor = true;
				}
			}
		}
		MulticastPlayDeathAnimationAndSound();
	}
	if (HealthComponent->Health > 0)
	{
		UMainCharAnimInstance* AnimInstance = Cast<UMainCharAnimInstance>(GetMesh()->GetAnimInstance());
		if (AnimInstance && AnimInstance->HitReactionMontage)
		{
			AnimInstance->Montage_Play(AnimInstance->HitReactionMontage);
		}

		PlayDamageSounds(false);
		MulticastPlayHitReactionAnimation();
		MulticastPlayHitReactionSound();

		UWidget_HealthBar* UserWidget = Cast<UWidget_HealthBar>(Widget_Component->GetUserWidgetObject());
		if (UserWidget)
		{
			UE_LOG(LogTemp, Warning, TEXT("Enemy Health is %f"), HealthComponent->Health);
		}
		UE_LOG(LogTemp, Warning, TEXT("Health is %f"), HealthComponent->Health);
	}
	return DamageCaused;
}

void ABaseMainCharacter::OnRep_HealthComponent()
{
	if (HealthComponent->Health <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Character Died.....!"));
		DisableInput(GetWorld()->GetFirstPlayerController());
		GetCharacterMovement()->DisableMovement();
		GetMesh()->SetSimulatePhysics(false);

		UMainCharAnimInstance* AnimInstance = Cast<UMainCharAnimInstance>(GetMesh()->GetAnimInstance());
		if (AnimInstance && AnimInstance->DeathAnimationMontage)
		{
			AnimInstance->Montage_Play(AnimInstance->DeathAnimationMontage);
		}
		PlayDamageSounds(true);
		DisableInput(GetWorld()->GetFirstPlayerController());

		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

		AAIController* AIController = Cast<AAIController>(GetController());
		if (AIController)
		{
			AIController->BrainComponent->StopLogic(TEXT("Character Died"));
		}

		if (HealthComponent)
		{
			HealthComponent->SetComponentTickEnabled(false);
		}

		if (Widget_Component)
		{
			Widget_Component->SetVisibility(false);
		}

		if (RestartWidget)
		{
			RestartWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), RestartWidget);
			if (RestartWidgetInstance)
			{
				RestartWidgetInstance->AddToViewport();

				APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
				if (PlayerController)
				{
					FInputModeUIOnly InputModeData;
					InputModeData.SetWidgetToFocus(RestartWidgetInstance->TakeWidget());
					InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

					PlayerController->SetInputMode(InputModeData);
					PlayerController->bShowMouseCursor = true;
				}
			}
		}
	}
	if (HealthComponent->Health > 0)
	{
		UMainCharAnimInstance* AnimInstance = Cast<UMainCharAnimInstance>(GetMesh()->GetAnimInstance());
		if (AnimInstance && AnimInstance->HitReactionMontage)
		{
			AnimInstance->Montage_Play(AnimInstance->HitReactionMontage);
		}

		PlayDamageSounds(false);

		UWidget_HealthBar* UserWidget = Cast<UWidget_HealthBar>(Widget_Component->GetUserWidgetObject());
		if (UserWidget)
		{
			//UserWidget->SetBarValuePercent(Health / Max_Health);
			UE_LOG(LogTemp, Warning, TEXT("Enemy Health is %f"), HealthComponent->Health);
		}
		UE_LOG(LogTemp, Warning, TEXT("Health is %f"), HealthComponent->Health);
	}

}

void ABaseMainCharacter::OnHealthChanged(float NewHealth, float NewMaxHealth)
{
	if (HealthBarWidgetComponent)
	{
		UWidget_HealthBar* HealthBarWidget = Cast<UWidget_HealthBar>(HealthBarWidgetComponent->GetUserWidgetObject());
		if (HealthBarWidget)
		{
			//HealthBarWidget->SetHealth(HealthComponent->Health, HealthComponent->MaxHealth);
		}
	}
}

void ABaseMainCharacter::OnAttackOverlapBegin(UPrimitiveComponent* const OverlappedComponent, AActor* const OtherActor, UPrimitiveComponent* const OtherComponent, int const OtherBodyIndex, bool const FromSweep, FHitResult const& SweepResult)
{
	if (OtherActor == this || !OtherActor)
	{
		return;
	}

	if (ABaseAIEnemy* const Enemy = Cast<ABaseAIEnemy>(OtherActor))
	{
		float const New_Health = Enemy->Get_Health() - Enemy->Get_Max_Health() * 0.30f;
		Enemy->Set_Health(New_Health);
		MainCharAnimInstance->PlayHitReactionMontage();
		//PlayHitReactionMontage();
	}
	else if (auto const Player = Cast<ABaseMainCharacter>(OtherActor))
	{
		auto const New_Health = Player->Get_Health() - Player->Get_Max_Health() * 0.10f;
		Player->Set_Health(New_Health);
		MainCharAnimInstance->PlayHitReactionMontage();
	}

	if (OtherActor != this && OtherActor->IsA<ABaseAIEnemy>())
	{
		ABaseAIEnemy* const Enemy = Cast<ABaseAIEnemy>(OtherActor);

		if (Enemy)
		{
			float const DamageToApply = MeleeDamage;
			Enemy->TakeDamage(DamageToApply, FDamageEvent(), GetController(), this);
			MainCharAnimInstance->PlayHitReactionMontage();
		}
	}
}

void ABaseMainCharacter::OnAttackOverlapEnd(UPrimitiveComponent* const OverlappedComponent, AActor* const OtherActor, UPrimitiveComponent* const OtherComponent, int const OtherBodyIndex)
{
}

void ABaseMainCharacter::PlayAttackSounds()
{
	if (AttackEffortSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, AttackEffortSound, GetActorLocation());
	}
	if (SwordSwingSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, SwordSwingSound, GetActorLocation());
	}
	if (ProjectileSpawnSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ProjectileSpawnSound, GetActorLocation());
	}
}

void ABaseMainCharacter::PlayDamageSounds(bool bIsDeath)
{
	if (bIsDeath && DeathSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());
	}
	else if (PainSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, PainSound, GetActorLocation());
	}
}
