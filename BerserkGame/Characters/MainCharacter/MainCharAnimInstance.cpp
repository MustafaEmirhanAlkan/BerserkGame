#include "BerserkGame/Characters/MainCharacter/MainCharAnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"
#include "BaseMainCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Pawn.h"

UMainCharAnimInstance::UMainCharAnimInstance()
{
	Speed = 0.0f;
	MaxWalkSpeed = 600.0f;
	bIsInAir = false;
	bIsMeleeAttacking = false;
	bIsRangedAttacking = false;
	BaseMainCharacter = nullptr;
	CurrentMontage = nullptr;
}

void UMainCharAnimInstance::UpdateAnimationProperties()
{
	APawn* Pawn = TryGetPawnOwner();
	if (Pawn)
	{
		Speed = Pawn->GetVelocity().Size();

		BaseMainCharacter = Cast<ABaseMainCharacter>(Pawn);
		if (BaseMainCharacter)
		{
			UCharacterMovementComponent* MovementComponent = BaseMainCharacter->GetCharacterMovement();
			if (MovementComponent)
			{
				bIsInAir = MovementComponent->IsFalling();
			}
			bIsMeleeAttacking = BaseMainCharacter->IsMeleeAttacking();
			bIsRangedAttacking = BaseMainCharacter->IsRangedAttacking();
		}
	}
}

void UMainCharAnimInstance::PlayMontage(UAnimMontage* Montage)
{
	if (Montage && BaseMainCharacter)
	{
		float PlayRate = Speed / BaseMainCharacter->GetCharacterMovement()->MaxWalkSpeed;
		Montage_Play(Montage, PlayRate);
		CurrentMontage = Montage;
	}
}

void UMainCharAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	AActor* OwningActor = GetOwningActor();

	if (OwningActor != nullptr)
	{
		Speed = OwningActor->GetVelocity().Size();
		ABaseMainCharacter* OwningCharacter = Cast<ABaseMainCharacter>(OwningActor);
		if (OwningCharacter != nullptr)
		{
			bIsInAir = OwningCharacter->GetCharacterMovement()->IsFalling();
		}
	}
}

void UMainCharAnimInstance::PlayAnimMontage(UAnimMontage* Montage, float InPlayRate)
{

}

void UMainCharAnimInstance::PlayStanding_1H_Magic_Attack_01()
{
	if (Standing_1H_Magic_Attack_01)
		Montage_Play(Standing_1H_Magic_Attack_01);
}
void UMainCharAnimInstance::PlaySword_And_Shield_Slash()
{
	if (Sword_And_Shield_Slash)
		Montage_Play(Sword_And_Shield_Slash);
}
void UMainCharAnimInstance::PlayHitReactionMontage()
{
	if (HitReactionMontage)
		Montage_Play(HitReactionMontage);
}
void UMainCharAnimInstance::PlayDeathAnimationMontage()
{
	if (DeathAnimationMontage)
		Montage_Play(DeathAnimationMontage);
}
void UMainCharAnimInstance::PlayThirdPersonIdle()
{
	if (ThirdPersonIdle)
		Montage_Play(ThirdPersonIdle);
}
void UMainCharAnimInstance::PlayThirdPersonWalk()
{
	if (ThirdPersonWalk)
		Montage_Play(ThirdPersonWalk);
}
void UMainCharAnimInstance::PlayForward_Right_Run()
{
	if (Forward_Right_Run)
		Montage_Play(Forward_Right_Run);
}
void UMainCharAnimInstance::PlayRight_Strafe_Walking()
{
	if (Right_Strafe_Walking)
		Montage_Play(Right_Strafe_Walking);
}
void UMainCharAnimInstance::PlayRun_Backward_Right()
{
	if (Run_Backward_Right)
		Montage_Play(Run_Backward_Right);
}
void UMainCharAnimInstance::PlayWalking_Backwards()
{
	if (Walking_Backwards)
		Montage_Play(Walking_Backwards);
}
void UMainCharAnimInstance::PlayForward_Left_Run()
{
	if (Forward_Left_Run)
		Montage_Play(Forward_Left_Run);
}
void UMainCharAnimInstance::PlayLeft_Strafe_Walking()
{
	if (Left_Strafe_Walking)
		Montage_Play(Left_Strafe_Walking);
}
void UMainCharAnimInstance::PlayRun_Backward_Left()
{
	if (Run_Backward_Left)
		Montage_Play(Run_Backward_Left);
}
void UMainCharAnimInstance::PlayThirdPersonRun()
{
	if (ThirdPersonRun)
		Montage_Play(ThirdPersonRun);
}
void UMainCharAnimInstance::PlayRight_Strafe_Running()
{
	if (Right_Strafe_Running)
		Montage_Play(Right_Strafe_Running);
}
void UMainCharAnimInstance::PlayRun_Backwards()
{
	if (Run_Backwards)
		Montage_Play(Run_Backwards);
}
void UMainCharAnimInstance::PlayLeft_Strafe_Running()
{
	if (Left_Strafe_Running)
		Montage_Play(Left_Strafe_Running);
}
void UMainCharAnimInstance::PlayLeft_Turn()
{
	if (Left_Turn)
		Montage_Play(Left_Turn);
}
void UMainCharAnimInstance::PlayRight_Turn()
{
	if (Right_Turn)
		Montage_Play(Right_Turn);
}

