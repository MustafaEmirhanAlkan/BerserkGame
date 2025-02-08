#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MainCharMeleeSword.generated.h"

class USkeletalMeshComponent;
class UBoxComponent;

UCLASS()
class BERSERKGAME_API AMainCharMeleeSword : public AActor
{
	GENERATED_BODY()

public:
	AMainCharMeleeSword();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
		USkeletalMeshComponent* SwordMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
		UBoxComponent* DamageBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
		UParticleSystemComponent* ParticleSystem;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		float SwordDamage;

	UFUNCTION()
		void OnAttackOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void SetOwnerChar(AActor* NewOwner);
	void SetDamage(float NewDamage);
	void EnableDamageBox();
	void DisableDamageBox();

	TArray<AActor*> HitActors;

private:
	AActor* OwnerChar;
};