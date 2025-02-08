#include "BerserkGame/Characters/EnemyCharacters/MeleeEnemy.h"
#include "AIController.h"
#include "BerserkGame/Characters/MainCharacter/BaseMainCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

AMeleeEnemy::AMeleeEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	StoppingDistance = 100.0f;
	MoveSpeed = 500.0f;

	GetCharacterMovement()->bOrientRotationToMovement = true; // Face in the direction we are moving
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 600.0f;
	GetCharacterMovement()->AirControl = 0.2f;
}

void AMeleeEnemy::BeginPlay()
{
	Super::BeginPlay();

	PlayerCharacter = Cast<ABaseMainCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));

	bCanRangedAttack = true;
	bCanMeleeAttack = true;
}

void AMeleeEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PlayerCharacter && FVector::Dist(GetActorLocation(), PlayerCharacter->GetActorLocation()) < StoppingDistance)
	{
		MoveToPlayer();
	}

	if (PlayerCharacter)
	{
		float const DistanceToPlayer = FVector::Dist(GetActorLocation(), PlayerCharacter->GetActorLocation());

		bCanMeleeAttack = DistanceToPlayer <= MeleeRange;

		bCanRangedAttack = DistanceToPlayer > MeleeRange && DistanceToPlayer <= ProjectileRange;

		AAIController* AICont = Cast<AAIController>(GetController());
		if (AICont)
		{
			UBlackboardComponent* BlackboardComp = AICont->GetBlackboardComponent();
			if (BlackboardComp)
			{
				BlackboardComp->SetValueAsBool(FName("CanMeleeAttack"), bCanMeleeAttack);
				BlackboardComp->SetValueAsBool(FName("CanRangedAttack"), bCanRangedAttack);
			}
		}
	}
}

void AMeleeEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

bool AMeleeEnemy::CanMeleeAttack()
{
	AAIController* AICont = Cast<AAIController>(GetController());
	if (AICont)
	{
		UBlackboardComponent* BlackboardComp = AICont->GetBlackboardComponent();
		if (BlackboardComp)
		{
			if (FVector::Dist(GetActorLocation(), PlayerCharacter->GetActorLocation()) <= MeleeRange)
			{
				BlackboardComp->SetValueAsBool(TEXT("CanMeleeAttack"), true);
			}
			else
			{
				BlackboardComp->SetValueAsBool(TEXT("CanMeleeAttack"), false);
			}
		}
	}
	return bCanMeleeAttack;
}

bool AMeleeEnemy::CanRangedAttack()
{
	AAIController* AICont = Cast<AAIController>(GetController());
	if (AICont)
	{
		UBlackboardComponent* BlackboardComp = AICont->GetBlackboardComponent();
		if (BlackboardComp)
		{
			bool bIsInMeleeRange = FVector::Dist(GetActorLocation(), PlayerCharacter->GetActorLocation()) <= MeleeRange;
			BlackboardComp->SetValueAsBool(TEXT("CanMeleeAttack"), bIsInMeleeRange);

			if (FVector::Dist(GetActorLocation(), PlayerCharacter->GetActorLocation()) <= ProjectileRange)
			{
				BlackboardComp->SetValueAsBool(TEXT("CanRangedAttack"), true);
			}
			else
			{
				BlackboardComp->SetValueAsBool(TEXT("CanRangedAttack"), false);
			}
		}
	}
	return bCanRangedAttack;
}

void AMeleeEnemy::MoveToPlayer()
{
	FVector Direction = (PlayerCharacter->GetActorLocation() - GetActorLocation()).GetSafeNormal();

	AddMovementInput(Direction, MoveSpeed * GetWorld()->GetDeltaSeconds());
}

void AMeleeEnemy::Attack()
{
	PlayAnimMontage(AttackAnimation);
}

