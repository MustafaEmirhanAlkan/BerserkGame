#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AC_HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChangedSignature, float, Health, float, MaxHealth);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BERSERKGAME_API UAC_HealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UAC_HealthComponent();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(ReplicatedUsing = OnRep_Health, EditAnywhere, BlueprintReadWrite, Category = "Health")
		float Health;

	UPROPERTY(ReplicatedUsing = OnRep_MaxHealth, EditAnywhere, BlueprintReadWrite, Category = "Health")
		float MaxHealth;

	UFUNCTION(BlueprintCallable, Category = "Health")
		void SetHealth(float NewHealth);

	UFUNCTION(BlueprintCallable, Category = "Health")
		float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Health")
		void SetMaxHealth(float NewMaxHealth);

	UFUNCTION(BlueprintCallable, Category = "Health")
		float GetMaxHealth() const;

	UPROPERTY(BlueprintReadOnly, Category = "Health")
		bool bIsDead;

	UFUNCTION(BlueprintPure, Category = "Health")
		bool GetIsDead() const;

	UPROPERTY(BlueprintAssignable, Category = "Health")
		FOnHealthChangedSignature OnHealthChanged;

	UFUNCTION()
		void OnRep_Health();

	UFUNCTION()
		void OnRep_MaxHealth();

	UFUNCTION(NetMulticast, Reliable)
		void MulticastHealthChanged(float NewHealth);
	void MulticastHealthChanged_Implementation(float NewHealth);

	UFUNCTION(NetMulticast, Reliable)
		void MLC_HealthChange();
		void MLC_HealthChange_Implementation();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};