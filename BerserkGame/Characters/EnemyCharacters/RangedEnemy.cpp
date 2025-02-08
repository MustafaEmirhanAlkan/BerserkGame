#include "BerserkGame/Characters/EnemyCharacters/RangedEnemy.h"
#include "BerserkGame/Characters/MainCharacter/BaseMainCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "BerserkGame/Characters/MainCharacter/Weapons/MainCharRangedProjectile.h"
#include "AIController.h"

ARangedEnemy::ARangedEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	StoppingDistance = 100.0f;
	MoveSpeed = 500.0f;
	AttackRange = 1000.0f;
	AttackCooldown = 2.0f;
	NextAttackTime = 0.0f;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 600.0f;
	GetCharacterMovement()->AirControl = 0.2f;
}

void ARangedEnemy::BeginPlay()
{
	Super::BeginPlay();
	PlayerCharacter = Cast<ABaseMainCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));

	bCanRangedAttack = true;
	bCanMeleeAttack = true;
}

void ARangedEnemy::Tick(float DeltaTime)
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

void ARangedEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}


void ARangedEnemy::MoveToPlayer()
{
	FVector Direction = (PlayerCharacter->GetActorLocation() - GetActorLocation()).GetSafeNormal();

	AddMovementInput(Direction, MoveSpeed * GetWorld()->GetDeltaSeconds());
}

void ARangedEnemy::Attack()
{
	if (ProjectileClass)
	{
		FVector Direction = (PlayerCharacter->GetActorLocation() - GetActorLocation()).GetSafeNormal();
		FRotator Rotation = Direction.Rotation();
		FVector SpawnLocation = GetActorLocation() + (Direction * 100.0f);

		GetWorld()->SpawnActor<AMainCharRangedProjectile>(ProjectileClass, SpawnLocation, Rotation);

		NextAttackTime = GetWorld()->GetTimeSeconds() + AttackCooldown;

		PlayAnimMontage(AttackAnimation);
	}
}

bool ARangedEnemy::CanRangedAttack()
{
	AAIController* AICont = Cast<AAIController>(GetController());
	if (AICont)
	{
		UBlackboardComponent* BlackboardComp = AICont->GetBlackboardComponent();
		if (BlackboardComp)
		{
			bool bIsInProjectileRange = FVector::Dist(GetActorLocation(), PlayerCharacter->GetActorLocation()) <= ProjectileRange;
			BlackboardComp->SetValueAsBool(TEXT("CanRangedAttack"), bIsInProjectileRange);

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

bool ARangedEnemy::CanMeleeAttack()
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

