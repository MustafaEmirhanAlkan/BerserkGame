#include "AttackAnimNotifyState.h"
#include "BerserkGame/Characters/MainCharacter/Weapons/MainCharMeleeSword.h"
#include "BerserkGame/Characters/MainCharacter/BaseMainCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/Engine.h"

void UAttackAnimNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
    if (MeshComp && MeshComp->GetOwner())
    {
        ABaseMainCharacter* Character = Cast<ABaseMainCharacter>(MeshComp->GetOwner());
        if (Character && Character->MeleeWeapon)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Notify Begin: Enabling Damage Box"));
            Character->MeleeWeapon->EnableDamageBox();
        }
    }
}

void UAttackAnimNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    if (MeshComp && MeshComp->GetOwner())
    {
        ABaseMainCharacter* Character = Cast<ABaseMainCharacter>(MeshComp->GetOwner());
        if (Character && Character->MeleeWeapon)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Notify End: Disabling Damage Box"));
            Character->MeleeWeapon->DisableDamageBox();

			FString HitActorsList = TEXT("Hit Actors at End of Attack: ");
			for (AActor* HitActor : Character->MeleeWeapon->HitActors)
			{
				HitActorsList += HitActor->GetName() + TEXT(", ");
			}
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, HitActorsList);

            //Listeyi resetleme.
            Character->MeleeWeapon->HitActors.Empty();
        }
    }
}