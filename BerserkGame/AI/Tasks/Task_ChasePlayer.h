#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "BerserkGame/Characters/EnemyCharacters/BaseAIEnemy.h"
#include "Task_ChasePlayer.generated.h"

UCLASS(Blueprintable)
class BERSERKGAME_API UTask_ChasePlayer : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UTask_ChasePlayer(FObjectInitializer const& ObjectInitializer);

	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
