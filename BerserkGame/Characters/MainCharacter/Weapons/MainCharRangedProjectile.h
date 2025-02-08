#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MainCharRangedProjectile.generated.h"

class UProjectileMovementComponent;
class UStaticMeshComponent;

UCLASS()
class BERSERKGAME_API AMainCharRangedProjectile : public AActor
{
	GENERATED_BODY()

public:

	AMainCharRangedProjectile();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
		class UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
		class UStaticMeshComponent* ProjectileMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
		class USphereComponent* DamageSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
		UParticleSystemComponent* ParticleSystem;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
		float ProjectileSpeed;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
		float ProjectileDamage = 35.0f;

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<UDamageType> DamageType;

	void SetDamage(float NewDamage);

	void IgnoreOwner();

	UFUNCTION()
		void OnAttackOverlapBegin(
			UPrimitiveComponent* const OverlappedComponent,
			AActor* const OtherActor,
			UPrimitiveComponent* const OtherComponent,
			int const OtherBodyIndex,
			bool const FromSweep,
			FHitResult const& SweepResult);

	void HomingTowards(AActor* TargetActor);

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
};

