#include "BerserkGame/Characters/EnemyCharacters/BaseAIEnemy.h"
#include "AITypes.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "BerserkGame/Components/Widget_HealthBar.h"
#include "Blueprint/UserWidget.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BerserkGame/Characters/MainCharacter/BaseMainCharacter.h"
#include "BerserkGame/Controller/BaseEnemyController.h"
#include "BerserkGame/AI/Blackboard/Enemy_Tags.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/Character.h"
#include "Navigation/PathFollowingComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Hearing.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"
#include "Runtime/Engine/Classes/Components/BoxComponent.h"
#include "Runtime/Engine/Classes/Components/CapsuleComponent.h"
#include "Runtime/Engine/Classes/Components/SphereComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "BerserkGame/Characters/MainCharacter/MainCharAnimInstance.h"

ABaseAIEnemy::ABaseAIEnemy() : Widget_Component(CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"))), Health(Max_Health)
{
	PrimaryActorTick.bCanEverTick = true;

	PlayerCollisionDetection = CreateDefaultSubobject<USphereComponent>(TEXT("Player Collision Detection"));
	PlayerCollisionDetection->SetupAttachment(RootComponent);

	PlayerAttackCollisionDetection = CreateDefaultSubobject<USphereComponent>(TEXT("Player Attack Collision Detection"));
	PlayerAttackCollisionDetection->SetupAttachment(RootComponent);

	BaseEnemyController = Cast<ABaseEnemyController>(GetController());
	if (BaseEnemyController)
	{
		BaseEnemyController->GetPathFollowingComponent()->OnRequestFinished.AddUObject(this, &ABaseAIEnemy::OnAIMoveCompleted);
	}
	setup_stimulus();

	if (Widget_Component)
	{
		Widget_Component->SetupAttachment(RootComponent);
		Widget_Component->SetWidgetSpace(EWidgetSpace::Screen);
		Widget_Component->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
		//static ConstructorHelpers::FClassFinder<UUserWidget> Widget_Class(TEXT("WidgetBlueprint'/Game/Blueprint/Widgets/Widget_HealthBar'"));
		//if (Widget_Class.Succeeded())
		//{
		//	Widget_Component->SetWidgetClass(Widget_Class.Class);
		//}
	}

	IsEnemyAlive = true;
}

void ABaseAIEnemy::MeleeAttack()
{
	FVector Start = GetActorLocation();
	FVector ForwardVector = GetActorForwardVector();
	FVector End = Start + ForwardVector * 0.0f;

	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	if (MainCharAnimInstance)
	{
		MainCharAnimInstance->PlaySword_And_Shield_Slash();

		ABaseMainCharacter* MainCharacter = Cast<ABaseMainCharacter>(this);
		if (MainCharacter && MainCharacter->AttackEffortSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, MainCharacter->AttackEffortSound, GetActorLocation());
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Animation instance is null."));
	}

	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams))
	{
		AActor* HitActor = HitResult.GetActor();
		if (HitActor)
		{
			HitActor->TakeDamage(MeleeDamage, FDamageEvent(), nullptr, this);
			MainCharAnimInstance->PlayHitReactionMontage();
		}
	}
}

float ABaseAIEnemy::Get_Health() const
{
	return HealthComponent->Health;
}

float ABaseAIEnemy::Get_Max_Health() const
{
	return HealthComponent->MaxHealth;
}

void ABaseAIEnemy::Set_Health(float const New_Health)
{
	HealthComponent->Health = New_Health;
	if (HealthComponent->Health <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Enemy Dead."));
		//GetWorld()->GetFirstPlayerController()->ConsoleCommand("Restart.");
		OnEnemyDeath();
	}
	UE_LOG(LogTemp, Warning, TEXT("Enemy's health is %f"), HealthComponent->Health);
}

void ABaseAIEnemy::OnEnemyDeath()
{
	IsEnemyAlive = false;

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetVisibility(false, true);
	GetMesh()->SetVisibility(false);
}

UBehaviorTree* ABaseAIEnemy::GetBehaviorTree() const
{
	return nullptr;
}

UAnimMontage* ABaseAIEnemy::GetMontage() const
{
	return nullptr;
}

void ABaseAIEnemy::setup_stimulus()
{
	stimulus = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("Stimulus"));
	stimulus->RegisterForSense(TSubclassOf<UAISense_Sight>());
	stimulus->RegisterWithPerceptionSystem();

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	if (stimulus)
	{
		stimulus->RegisterForSense(TSubclassOf<UAISense_Sight>());
		stimulus->RegisterWithPerceptionSystem();
	}
}

void ABaseAIEnemy::On_MeleeEnemyAttack()
{
	if (Distraction_Sound)
	{
		FVector const Location = GetActorLocation();
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), Distraction_Sound, Location);
		UAISense_Hearing::ReportNoiseEvent(GetWorld(), Location, 1.0f, this, 0.0f, Tags::noise_tag);
		PlaySword_And_Shield_Slash();
	}

	if (MainCharAnimInstance)
	{
		MainCharAnimInstance->PlaySword_And_Shield_Slash();
	}
}

void ABaseAIEnemy::On_RangedEnemyAttack()
{
	if (Distraction_Sound)
	{
		FVector const Location = GetActorLocation();
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), Distraction_Sound, Location);
		UAISense_Hearing::ReportNoiseEvent(GetWorld(), Location, 1.0f, this, 0.0f, Tags::noise_tag);
		PlayStanding_1H_Magic_Attack_01();
	}

	if (MainCharAnimInstance)
	{
		MainCharAnimInstance->PlayStanding_1H_Magic_Attack_01();
	}
}

void ABaseAIEnemy::On_Distract()
{
}

void ABaseAIEnemy::OnAIMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	if (this->Controller)
	{
		if (!PlayerDetected)
		{
			BaseEnemyController->RandomPatrol();
		}
		else if (PlayerDetected && CanAttackPlayer)
		{
			StopSeekingPlayer();
			MainCharAnimInstance->Montage_Play(EnemyAttackAnimation);
			PlaySword_And_Shield_Slash();
			PlayStanding_1H_Magic_Attack_01();
			PlayHitReactionMontage();
		}
		else if (PlayerDetected && CanAttackPlayer)
		{
			StopSeekingPlayer();
			UE_LOG(LogTemp, Warning, TEXT("Player ATTACKED"));
		}
	}
	PlayerCollisionDetection->OnComponentBeginOverlap.AddDynamic(this,
		&ABaseAIEnemy::OnPlayerDetectedOverlapBegin);

	PlayerCollisionDetection->OnComponentEndOverlap.AddDynamic(this,
		&ABaseAIEnemy::OnPlayerDetectedOverlapEnd);

	PlayerAttackCollisionDetection->OnComponentBeginOverlap.AddDynamic(this,
		&ABaseAIEnemy::OnPlayerAttackOverlapBegin);

	PlayerAttackCollisionDetection->OnComponentEndOverlap.AddDynamic(this,
		&ABaseAIEnemy::OnPlayerAttackOverlapEnd);
}

void ABaseAIEnemy::MoveToPlayer()
{
	BaseEnemyController->MoveToLocation(PlayerREF->GetActorLocation(), StoppingDistance, true);
}

void ABaseAIEnemy::SeekPlayer()
{
	MoveToPlayer();
	GetWorld()->GetTimerManager().SetTimer(SeekPlayerTimerHandle, this, &ABaseAIEnemy::SeekPlayer, 0.25f, true);
}

void ABaseAIEnemy::StopSeekingPlayer()
{
	GetWorld()->GetTimerManager().ClearTimer(SeekPlayerTimerHandle);
}

void ABaseAIEnemy::OnPlayerDetectedOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	PlayerREF = Cast<ABaseAIEnemy>(OtherActor);
	if (PlayerREF)
	{
		PlayerDetected = true;
		SeekPlayer();
	}
}

void ABaseAIEnemy::OnPlayerDetectedOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	PlayerREF = Cast<ABaseAIEnemy>(OtherActor);
	if (PlayerREF)
	{
		PlayerDetected = false;
		StopSeekingPlayer();
		BaseEnemyController->RandomPatrol();
	}
}

void ABaseAIEnemy::OnPlayerAttackOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	PlayerREF = Cast<ABaseAIEnemy>(OtherActor);
	if (PlayerREF)
	{
		CanAttackPlayer = true;
	}
}

void ABaseAIEnemy::OnPlayerAttackOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	PlayerREF = Cast<ABaseAIEnemy>(OtherActor);
	if (PlayerREF)
	{
		CanAttackPlayer = false;

		MainCharAnimInstance->Montage_Stop(0.0f, EnemyAttackAnimation);

		SeekPlayer();
	}
}

void ABaseAIEnemy::OnDealDamageOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	PlayerREF = Cast<ABaseAIEnemy>(OtherActor);
	if (PlayerREF && CanDealDamage)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player Damaged"));
	}
}

void ABaseAIEnemy::PlaySword_And_Shield_Slash()
{
	if (MainCharAnimInstance)
	{
		MainCharAnimInstance->PlaySword_And_Shield_Slash();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Melee AnimInstance is null."));
	}
}

void ABaseAIEnemy::PlayStanding_1H_Magic_Attack_01()
{
	if (MainCharAnimInstance)
	{
		MainCharAnimInstance->PlayStanding_1H_Magic_Attack_01();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Ranged AnimInstance is null."));
	}
}

void ABaseAIEnemy::PlayHitReactionMontage()
{
	if (MainCharAnimInstance)
	{
		MainCharAnimInstance->PlayHitReactionMontage();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Avoid AnimInstance is null."));
	}
}