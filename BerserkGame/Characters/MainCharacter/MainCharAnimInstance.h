#pragma once

#include "Animation/AnimInstance.h"
#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Animation/AnimInstance.h"
#include "MainCharAnimInstance.generated.h"

class UAnimMontage;

UCLASS(transient, Blueprintable, hideCategories = AnimInstance, BlueprintType)
class BERSERKGAME_API UMainCharAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UMainCharAnimInstance();

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float Speed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
		float MaxWalkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
		bool bIsInAir;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MeleeAttack")
		bool bIsMeleeAttacking;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RangedAttack")
		bool bIsRangedAttacking;

	UFUNCTION(BlueprintCallable, Category = "Animation")
		void UpdateAnimationProperties();

	UFUNCTION(BlueprintCallable, Category = "Animation")
		void PlayMontage(class UAnimMontage* Montage);

	UFUNCTION(BlueprintCallable, Category = "Animation")
		void PlayAnimMontage(UAnimMontage* Montage, float InPlayRate);

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation")
		class UMainCharAnimInstance* MainCharAnimInstance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
		UAnimMontage* Standing_1H_Magic_Attack_01;
	void PlayStanding_1H_Magic_Attack_01();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
		UAnimMontage* Sword_And_Shield_Slash;
	void PlaySword_And_Shield_Slash();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
		UAnimMontage* HitReactionMontage;

	UFUNCTION(BlueprintCallable, Category = "Animation")
		void PlayHitReactionMontage();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
		UAnimMontage* DeathAnimationMontage;
	void PlayDeathAnimationMontage();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
		UAnimMontage* ThirdPersonIdle;
	void PlayThirdPersonIdle();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
		UAnimMontage* ThirdPersonWalk;
	void PlayThirdPersonWalk();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
		UAnimMontage* Forward_Right_Run;
	void PlayForward_Right_Run();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
		UAnimMontage* Right_Strafe_Walking;
	void PlayRight_Strafe_Walking();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
		UAnimMontage* Run_Backward_Right;
	void PlayRun_Backward_Right();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
		UAnimMontage* Walking_Backwards;
	void PlayWalking_Backwards();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
		UAnimMontage* Forward_Left_Run;
	void PlayForward_Left_Run();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
		UAnimMontage* Left_Strafe_Walking;
	void PlayLeft_Strafe_Walking();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
		UAnimMontage* Run_Backward_Left;
	void PlayRun_Backward_Left();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
		UAnimMontage* ThirdPersonRun;
	void PlayThirdPersonRun();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
		UAnimMontage* Right_Strafe_Running;
	void PlayRight_Strafe_Running();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
		UAnimMontage* Run_Backwards;
	void PlayRun_Backwards();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
		UAnimMontage* Left_Strafe_Running;
	void PlayLeft_Strafe_Running();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
		UAnimMontage* Left_Turn;
	void PlayLeft_Turn();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
		UAnimMontage* Right_Turn;
	void PlayRight_Turn();
	
protected:
	UPROPERTY(BlueprintReadOnly, Category = "Animation")
		class ABaseMainCharacter* BaseMainCharacter;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
		UAnimMontage* CurrentMontage;
};