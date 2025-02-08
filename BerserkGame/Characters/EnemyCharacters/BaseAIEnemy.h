#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimMontage.h"
#include "BerserkGame/Characters/MainCharacter/BaseMainCharacter.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BerserkGame/AI/Combat/CombatInterface.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Sound/SoundBase.h"
#include "BaseAIEnemy.generated.h"

UCLASS()
class BERSERKGAME_API ABaseAIEnemy : public ABaseMainCharacter
{
	GENERATED_BODY()

public:
	ABaseAIEnemy();

	float Get_Health() const;
	float Get_Max_Health() const;
	void Set_Health(float const New_Health);

	void PlaySword_And_Shield_Slash();
	void PlayStanding_1H_Magic_Attack_01();
	void PlayHitReactionMontage();
	void MeleeAttack();

	bool PlayerDetected;
	bool CanAttackPlayer;

	class ABaseAIEnemy* PlayerREF;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
		bool Attack = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		USoundBase* Distraction_Sound;

	UPROPERTY(BlueprintReadWrite)
		bool CanDealDamage;

	UPROPERTY(EditDefaultsOnly)
		bool CanEverMeleeAttack;

	UPROPERTY(EditDefaultsOnly)
		bool CanEverRangedAttack;

	UPROPERTY(EditAnywhere)
		USphereComponent* PlayerCollisionDetection;

	UPROPERTY(EditAnywhere)
		USphereComponent* PlayerAttackCollisionDetection;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
		UAnimMontage* Sword_And_Shield_Slash;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
		UAnimMontage* Standing_1H_Magic_Attack_01;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
		UAnimMontage* AvoidBack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		float MeleeRange = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		float ProjectileRange = 1000.0f;

	UPROPERTY(BlueprintReadWrite, Category = "Status")
		bool IsEnemyAlive;

	UBehaviorTree* GetBehaviorTree() const;
	UAnimMontage* GetMontage() const;

protected:
	virtual void OnEnemyDeath();

private:
	class UAIPerceptionStimuliSourceComponent* stimulus;
	class ABaseEnemyController* BaseEnemyController;

	void setup_stimulus();
	void On_MeleeEnemyAttack();
	void On_RangedEnemyAttack();
	void On_Distract();

	class UWidgetComponent* Widget_Component;
	float const Max_Health = 100.0f;
	float Health = 100.0f;

	void OnAIMoveCompleted(struct FAIRequestID RequestID, const struct FPathFollowingResult& Result);

	void MoveToPlayer();

	void SeekPlayer();

	void StopSeekingPlayer();

	void OnPlayerDetectedOverlapBegin(class UPrimitiveComponent* OverlappedComp,
		class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void OnPlayerDetectedOverlapEnd(class UPrimitiveComponent* OverlappedComp,
		class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

	void OnPlayerAttackOverlapBegin(class UPrimitiveComponent* OverlappedComp,
		class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void OnPlayerAttackOverlapEnd(class UPrimitiveComponent* OverlappedComp,
		class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

	void OnDealDamageOverlapBegin(class UPrimitiveComponent* OverlappedComp,
		class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere)
		float StoppingDistance = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = true))
		UBehaviorTree* behavior_tree;

	UPROPERTY(EditAnywhere)
		UAnimMontage* EnemyAttackAnimation;

	FTimerHandle SeekPlayerTimerHandle;
};