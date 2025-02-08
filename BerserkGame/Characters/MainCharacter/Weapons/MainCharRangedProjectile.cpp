#include "BerserkGame/Characters/MainCharacter/Weapons/MainCharRangedProjectile.h"
#include "BerserkGame/Characters/MainCharacter/BaseMainCharacter.h"
#include "BerserkGame/Characters/EnemyCharacters/BaseAIEnemy.h"
#include "BerserkGame/Characters/MainCharacter/MainCharAnimInstance.h"
#include "BerserkGame/GameModes/BerserkGame.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "BerserkGame/Components/Widget_HealthBar.h"

AMainCharRangedProjectile::AMainCharRangedProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	RootComponent = ProjectileMesh;

	DamageSphere = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = RootComponent;
	ProjectileMovement->InitialSpeed = ProjectileSpeed;
	ProjectileMovement->MaxSpeed = ProjectileSpeed;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.0f;

	ParticleSystem = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSystem"));
	ParticleSystem->SetupAttachment(RootComponent);

	DamageSphere->OnComponentBeginOverlap.AddDynamic(this, &AMainCharRangedProjectile::OnAttackOverlapBegin);
	DamageSphere->SetupAttachment(RootComponent);

	ProjectileSpeed = 500.0f;
	ProjectileDamage = 35.0f;

	DamageSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	DamageSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AMainCharRangedProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (AActor* ProjectileOwner = GetOwner())
	{
		DamageSphere->IgnoreActorWhenMoving(ProjectileOwner, true);
	}
}

void AMainCharRangedProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMainCharRangedProjectile::SetDamage(float NewDamage)
{
	ProjectileDamage = NewDamage;
}

void AMainCharRangedProjectile::IgnoreOwner()
{
	if (AActor* ProjectileOwner = GetOwner())
	{
		DamageSphere->IgnoreActorWhenMoving(ProjectileOwner, true);
	}
}

void AMainCharRangedProjectile::OnAttackOverlapBegin(UPrimitiveComponent* const OverlappedComponent, AActor* const OtherActor, UPrimitiveComponent* const OtherComponent, int const OtherBodyIndex, bool const FromSweep, FHitResult const& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Projectile Overlap with %s"), *OtherActor->GetName());

	ABaseMainCharacter* const OwnerChar = Cast<ABaseMainCharacter>(GetOwner());
	if (!OwnerChar || !OwnerChar->HasAuthority() || OtherActor == OwnerChar)
	{
		return;
	}

	ABaseMainCharacter* const mainChar_ = Cast<ABaseMainCharacter>(OtherActor);
	if (mainChar_)
	{
		mainChar_->InflictDamage(ProjectileDamage, FDamageEvent(), GetInstigatorController(), this);
		Destroy();
		return;
	}

	ABaseAIEnemy* const enemy_ = Cast<ABaseAIEnemy>(OtherActor);
	if (enemy_ && enemy_->IsEnemyAlive)
	{
		enemy_->InflictDamage(ProjectileDamage, FDamageEvent(), GetInstigatorController(), this);
		Destroy();
		return;
	}
}

void AMainCharRangedProjectile::HomingTowards(AActor* TargetActor)
{
	if (TargetActor)
	{
		ABaseMainCharacter* MainCharacterTarget = Cast<ABaseMainCharacter>(TargetActor);
		if (MainCharacterTarget && !MainCharacterTarget->GetIsDeath())
		{
			ABaseAIEnemy* Enemy = Cast<ABaseAIEnemy>(TargetActor);
			if (Enemy && !Enemy->IsEnemyAlive)
			{
				UE_LOG(LogTemp, Warning, TEXT("Target is dead. Homing aborted."));
				return;
			}

			FVector HomingDirection = (TargetActor->GetActorLocation() - GetActorLocation()).GetSafeNormal();
			ProjectileMovement->Velocity = HomingDirection * ProjectileMovement->InitialSpeed;
		}
	}
}