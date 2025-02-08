#pragma once

#include "CoreMinimal.h"
#include "BerserkGame/Characters/EnemyCharacters/BaseAIEnemy.h"
#include "RangedEnemy.generated.h"

UCLASS()
class BERSERKGAME_API ARangedEnemy : public ABaseAIEnemy
{
	GENERATED_BODY()

public:
	ARangedEnemy();
	bool bCanPerformMeleeAttack;
	bool bCanPerformRangedAttack;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	bool CanRangedAttack();
	bool CanMeleeAttack();
	void MoveToPlayer();
	void Attack();

	UPROPERTY(EditAnywhere, Category = "Ranged Attack")
		float AttackRange;

	UPROPERTY(EditAnywhere, Category = "Ranged Attack")
		float AttackCooldown;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Attack")
		bool bCanRangedAttack;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Attack")
		bool bCanMeleeAttack;

	UPROPERTY(EditAnywhere, Category = "Ranged Attack")
		TSubclassOf<class AMainCharRangedProjectile> ProjectileClass;

	class ABaseMainCharacter* PlayerCharacter;
	float StoppingDistance;
	float MoveSpeed;
	float NextAttackTime;
	UAnimMontage* AttackAnimation;
};
