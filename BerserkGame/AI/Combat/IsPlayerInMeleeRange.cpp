#include "BerserkGame/AI/Combat/IsPlayerInMeleeRange.h"
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

UIsPlayerInMeleeRange::UIsPlayerInMeleeRange()
{
	bNotifyTick = true;
	NodeName = TEXT("Is Player In Melee Range");
}

void UIsPlayerInMeleeRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
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
	const bool bPlayerInMeleeRange = DistanceToPlayer <= EnemyCharacter->MeleeRange;

	if (bPlayerInMeleeRange)
	{
		UE_LOG(LogTemp, Error, TEXT("player_is_in_melee_range = true"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("player_is_in_melee_range = false"));
	}

	UE_LOG(LogTemp, Error, TEXT("Melee_Range %f"), Melee_Range);
	UE_LOG(LogTemp, Error, TEXT("GetDistanceTo %f"), EnemyCharacter->GetDistanceTo(Player));

	Cont->GetBlackboardComponent()->SetValueAsBool(Blackboard_Keys::player_is_in_melee_range, bPlayerInMeleeRange);

	if (bPlayerInMeleeRange)
	{
		EnemyCharacter->MeleeAttack();
	}
}