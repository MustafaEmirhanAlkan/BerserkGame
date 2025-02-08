#include "BerserkGame/Components/AC_HealthComponent.h"
#include "Net/UnrealNetwork.h"

UAC_HealthComponent::UAC_HealthComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	Health = 100.0f;
	MaxHealth = 100.0f;
	bIsDead = false;

	//MP
	SetIsReplicatedByDefault(true);
}

void UAC_HealthComponent::BeginPlay()
{
	Super::BeginPlay();

	//MP
	if (GetOwner()->HasAuthority())
	{
		OnHealthChanged.Broadcast(Health, MaxHealth);
	}
}

void UAC_HealthComponent::SetHealth(float NewHealth)
{
	if (GetOwner()->HasAuthority()) //MP
	{
		if (NewHealth != Health)
		{
			Health = FMath::Clamp(NewHealth, 0.0f, MaxHealth);
			OnRep_Health();
			OnHealthChanged.Broadcast(Health, MaxHealth);
			bIsDead = Health <= 0.0f;

			MulticastHealthChanged(Health); //MP
		}
	}
}

float UAC_HealthComponent::GetHealth() const
{
	return Health;
}

void UAC_HealthComponent::SetMaxHealth(float NewMaxHealth)
{
	if (GetOwner()->HasAuthority()) //MP
	{
		MaxHealth = FMath::Max(NewMaxHealth, 1.0f);
		Health = FMath::Min(Health, MaxHealth);
		OnRep_Health();
		OnHealthChanged.Broadcast(Health, MaxHealth);

		MulticastHealthChanged(Health); //MP
	}
}

float UAC_HealthComponent::GetMaxHealth() const
{
	return MaxHealth;
}

bool UAC_HealthComponent::GetIsDead() const
{
	return bIsDead;
} 

void UAC_HealthComponent::OnRep_Health()
{
	OnHealthChanged.Broadcast(Health, MaxHealth);
}

void UAC_HealthComponent::OnRep_MaxHealth()
{
	OnHealthChanged.Broadcast(Health, MaxHealth);
}

void UAC_HealthComponent::MulticastHealthChanged_Implementation(float NewHealth)
{
	Health = NewHealth;
	OnHealthChanged.Broadcast(Health, MaxHealth);
}

void UAC_HealthComponent::MLC_HealthChange_Implementation()
{
	OnHealthChanged.Broadcast(Health, MaxHealth);
}

void UAC_HealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UAC_HealthComponent, Health);
	DOREPLIFETIME(UAC_HealthComponent, MaxHealth); 
}