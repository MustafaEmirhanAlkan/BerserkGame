#include "BerserkGame/NotifyStates/AnimNotify_SpawnProjectile.h"
#include "BerserkGame/Characters/MainCharacter/BaseMainCharacter.h"
#include "BerserkGame/Characters/MainCharacter/Weapons/MainCharRangedProjectile.h"
#include "Kismet/GameplayStatics.h"

void UAnimNotify_SpawnProjectile::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (MeshComp && MeshComp->GetOwner())
	{
		ABaseMainCharacter* Character = Cast<ABaseMainCharacter>(MeshComp->GetOwner());
		if (Character)
		{
			FVector HandLocation = MeshComp->GetSocketLocation(FName("Hand_R"));
			FRotator HandRotation = MeshComp->GetSocketRotation(FName("Hand_R"));

			UWorld* World = Character->GetWorld();
			if (World)
			{
				World->SpawnActor<AMainCharRangedProjectile>(AMainCharRangedProjectile::StaticClass(), HandLocation, HandRotation);
			}
		}
	}
}