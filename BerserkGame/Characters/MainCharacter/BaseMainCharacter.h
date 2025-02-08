#pragma once

#include "Animation/AnimMontage.h"
#include "BerserkGame/Components/AC_HealthComponent.h"
#include "BerserkGame/Characters/MainCharacter/Weapons/MainCharRangedProjectile.h"
#include "BerserkGame/Characters/MainCharacter/Weapons/MainCharMeleeSword.h"
#include "Blueprint/UserWidget.h"
#include "Components/BoxComponent.h"
#include "Components/InputComponent.h"
#include "Camera/CameraComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MainCharAnimInstance.h"
#include "GoogleInstantPreview/instant_preview_server.h"
#include "BaseMainCharacter.generated.h"

class AMainCharMeleeSword;

UCLASS()
class BERSERKGAME_API ABaseMainCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABaseMainCharacter();

	UAnimMontage* Get_montage() const;

protected:
	virtual void BeginPlay() override;

	bool Attacking;
	bool AttackDamaging;
	bool MovingForward;
	bool MovingBackwards;

	virtual void Attack();

	void AttachSwordToCharacter();

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		TSubclassOf<AMainCharMeleeSword> SwordClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
		AMainCharMeleeSword* EquippedSword;

	UFUNCTION(BlueprintCallable, Category = "Combat")
		virtual void AttackLunge();

	UFUNCTION(BlueprintCallable, Category = "Combat")
		virtual void EndAttack();

	UFUNCTION(BlueprintCallable, Category = "Combat")
		virtual void SetAttackDamaging(bool Damaging);

	float LastRotationSpeed;

private:
	// MP
	void MeleeAttack_Internal();
	void RangedAttack_Internal();

	// Internal attack handling
	void InflictDamage_Internal(float DamageAmount, AActor* DamageCauser);

public:
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerMeleeAttack();
	void ServerMeleeAttack_Implementation();
	bool ServerMeleeAttack_Validate();

	UFUNCTION(BlueprintCallable)
		void MeleeAttack();

	UFUNCTION(NetMulticast, Reliable)
		void MulticastPlayMeleeAttackAnimation();
	void MulticastPlayMeleeAttackAnimation_Implementation();

	UFUNCTION(NetMulticast, Reliable)
		void MLC_MeleeAttack();

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerRangedAttack();
	void ServerRangedAttack_Implementation();
	bool ServerRangedAttack_Validate();

	UFUNCTION(BlueprintCallable)
		void RangedAttack();

	UFUNCTION(NetMulticast, Reliable)
		void MulticastPlayRangedAttackAnimation();
	void MulticastPlayRangedAttackAnimation_Implementation();

	UFUNCTION(NetMulticast, Reliable)
		void MLC_RangedAttack();

	UPROPERTY(ReplicatedUsing = OnRep_LastMeleeAttackTime)
		float LastMeleeAttackTime;

	UFUNCTION()
		void OnRep_LastMeleeAttackTime();

	UPROPERTY(ReplicatedUsing = OnRep_LastRangedAttackTime)
		float LastRangedAttackTime;

	UFUNCTION()
		void OnRep_LastRangedAttackTime();

	UFUNCTION(NetMulticast, Reliable)
		void MulticastPlayHitReactionSound();
	void MulticastPlayHitReactionSound_Implementation();

	UFUNCTION(NetMulticast, Reliable)
		void MulticastPlayHitReactionAnimation();
	void MulticastPlayHitReactionAnimation_Implementation();

	UFUNCTION(NetMulticast, Reliable)
		void MulticastPlayDeathAnimationAndSound();
	void MulticastPlayDeathAnimationAndSound_Implementation();

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerInflictDamage(float DamageAmount, AActor* DamageCauser);

	void ServerInflictDamage_Implementation(float DamageAmount, AActor* DamageCauser);
	bool ServerInflictDamage_Validate(float DamageAmount, AActor* DamageCauser);

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<AMainCharRangedProjectile> ProjectileInstance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
		class UMainCharAnimInstance* MainCharAnimInstance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
		float MeleeDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
		float FireballDamage;

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
		bool bIsMeleeAttacking;

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
		bool bIsRangedAttacking;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
		float BaseTurnRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
		float BaseLookUpRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
		float TurnRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
		float LookUpRate;

	UFUNCTION(BlueprintCallable, Category = "Combat")
		bool IsMeleeAttacking() const { return bIsMeleeAttacking; }

	UFUNCTION(BlueprintCallable, Category = "Combat")
		bool IsRangedAttacking() const { return bIsRangedAttacking; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		UAnimMontage* MeleeMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		UAnimMontage* RangedMontage;

	void On_MeleeEnemyAttack();
	void On_RangedEnemyAttack();

	float Get_Health() const;
	float Get_Max_Health() const;
	void Set_Health(float const New_Health);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		class UWidgetComponent* Widget_Component;

	UFUNCTION(BlueprintCallable, Category = "Damage")
		float InflictDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = "true"))
		UAnimMontage* montage;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	//	class UAC_HealthComponent* HealthComponent;

	UPROPERTY(ReplicatedUsing = OnRep_HealthComponent, VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
		UAC_HealthComponent* HealthComponent;

	UFUNCTION()
		void OnRep_HealthComponent();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
		class UWidgetComponent* HealthBarWidgetComponent;

	UFUNCTION()
		void OnHealthChanged(float NewHealth, float NewMaxHealth);

	UFUNCTION()
		void OnAttackOverlapBegin(
			UPrimitiveComponent* const OverlappedComponent,
			AActor* const OtherActor,
			UPrimitiveComponent* const OtherComponent,
			int const OtherBodyIndex,
			bool const FromSweep,
			FHitResult const& SweepResult);

	UFUNCTION()
		void OnAttackOverlapEnd(
			UPrimitiveComponent* const OverlappedComponent,
			AActor* const OtherActor,
			UPrimitiveComponent* const OtherComponent,
			int const OtherBodyIndex);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		AMainCharMeleeSword* MeleeWeapon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
		float MeleeAttackCooldownTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
		float RangedAttackCooldownTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
		USoundBase* AttackEffortSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
		USoundBase* SwordSwingSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
		USoundBase* ProjectileSpawnSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
		USoundBase* PainSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
		USoundBase* DeathSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
		TSubclassOf<UUserWidget> RestartWidget;

	void PlayAttackSounds();
	void PlayDamageSounds(bool bIsDeath);
	bool GetIsDeath() const;

private:
	UPROPERTY()
		class UWidget_HealthBar* HealthBarWidgetInstance;

	UUserWidget* RestartWidgetInstance;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};