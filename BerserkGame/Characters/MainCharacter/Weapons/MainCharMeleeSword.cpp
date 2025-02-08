#include "MainCharMeleeSword.h"
#include "BerserkGame/Characters/MainCharacter/BaseMainCharacter.h"
#include "BerserkGame/Characters/EnemyCharacters/BaseAIEnemy.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Engine/Engine.h"

AMainCharMeleeSword::AMainCharMeleeSword()
{
	PrimaryActorTick.bCanEverTick = true;

	SwordMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SwordMesh"));
	RootComponent = SwordMesh;

	DamageBox = CreateDefaultSubobject<UBoxComponent>(TEXT("DamageBox"));
	DamageBox->SetupAttachment(SwordMesh);
	DamageBox->OnComponentBeginOverlap.AddDynamic(this, &AMainCharMeleeSword::OnAttackOverlapBegin);
	DamageBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);  // Initially disabled
	DamageBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	DamageBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	DamageBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	ParticleSystem = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSystem"));
	ParticleSystem->SetupAttachment(RootComponent);

	SwordDamage = 20.0f;
	Tags.Add(FName("MeleeSword"));
}

void AMainCharMeleeSword::BeginPlay()
{
	Super::BeginPlay();

	DamageBox->OnComponentBeginOverlap.AddDynamic(this, &AMainCharMeleeSword::OnAttackOverlapBegin);
}

void AMainCharMeleeSword::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMainCharMeleeSword::OnAttackOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//List controls Char or Actor. Calculates with Other Actor. Other Actor kimse vurdugu kisiyi listeye alacak ve bir daha vurmayacak.
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("OnAttackOverlapBegin: %s"), *OtherActor->GetName()));

	if (!OwnerChar || OtherActor == OwnerChar)
	{
		return;
	}

	if (!OwnerChar->HasAuthority())
	{
		return;
	}

	//Liste kontrolu icin bir if daha ekle.
	if (HitActors.Contains(OtherActor))
	{
		return;
	}
	HitActors.Add(OtherActor);

	FString HitActorsList = TEXT("Current Hit Actors: ");
	for (AActor* HitActor : HitActors)
	{
		HitActorsList += HitActor->GetName() + TEXT(", ");
	}
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, HitActorsList);

	if (OtherActor)
	{
		ABaseMainCharacter* MainChar = Cast<ABaseMainCharacter>(OtherActor);
		if (MainChar)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("InflictDamage on MainChar"));
			MainChar->InflictDamage(SwordDamage, FDamageEvent(), nullptr, this);
		}
	}
}

void AMainCharMeleeSword::SetOwnerChar(AActor* NewOwner)
{
	OwnerChar = NewOwner;
}

void AMainCharMeleeSword::SetDamage(float NewDamage)
{
	SwordDamage = NewDamage;
}

void AMainCharMeleeSword::EnableDamageBox()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("EnableDamageBox"));
	DamageBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AMainCharMeleeSword::DisableDamageBox()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("DisableDamageBox"));
	DamageBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}