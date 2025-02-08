#include "BerserkGame/AI/Combat/IsPlayerInProjectileRange.h"
#include "BerserkGame/Controller/BaseEnemyController.h"
#include "BerserkGame/Characters/EnemyCharacters/BaseAIEnemy.h"
#include "BerserkGame/Characters/MainCharacter/BaseMainCharacter.h"
#include "BerserkGame/Characters/MainCharacter/MainCharAnimInstance.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BerserkGame/AI/Blackboard/Blackboard_Keys.h"
#include "GameFramework/Character.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Engine/World.h"

UIsPlayerInProjectileRange::UIsPlayerInProjectileRange()
{
	bNotifyTick = true;
	NodeName = TEXT("Is Player In Projectile Range");
}

void UIsPlayerInProjectileRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	ABaseEnemyController* const Cont = Cast<ABaseEnemyController>(OwnerComp.GetAIOwner());
	if (!Cont)
	{
		return;
	}

	ABaseAIEnemy* const EnemyCharacter = Cast<ABaseAIEnemy>(Cont->GetPawn());
	if (!EnemyCharacter)
	{
		return;
	}

	ACharacter* const Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (!Player)
	{
		return;
	}

	const float DistanceToPlayer = EnemyCharacter->GetDistanceTo(Player);
	const bool bPlayerInProjectileRange = DistanceToPlayer <= EnemyCharacter->ProjectileRange;

	if (bPlayerInProjectileRange)
	{
		UE_LOG(LogTemp, Error, TEXT("player_is_in_projectile_range = true"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("player_is_in_projectile_range = false"));
	}

	UE_LOG(LogTemp, Error, TEXT("Projectile_Range %f"), Projectile_Range);
	UE_LOG(LogTemp, Error, TEXT("GetDistanceTo %f"), EnemyCharacter->GetDistanceTo(Player));

	Cont->GetBlackboardComponent()->SetValueAsBool(Blackboard_Keys::player_is_in_projectile_range, bPlayerInProjectileRange);
}
