#pragma once

#include "CoreMinimal.h"
#include "BerserkGame/Characters/EnemyCharacters/BaseAIEnemy.h"
#include "MeleeEnemy.generated.h"

UCLASS()
class BERSERKGAME_API AMeleeEnemy : public ABaseAIEnemy
{
	GENERATED_BODY()

public:
	AMeleeEnemy();
	bool bCanPerformMeleeAttack;
	bool bCanPerformRangedAttack;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	bool CanMeleeAttack();
	bool CanRangedAttack();
	void MoveToPlayer();
	void Attack();
	class ABaseMainCharacter* PlayerCharacter;
	float StoppingDistance;
	float MoveSpeed;
	UAnimMontage* AttackAnimation;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Attack")
		bool bCanMeleeAttack;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Attack")
		bool bCanRangedAttack;
};
