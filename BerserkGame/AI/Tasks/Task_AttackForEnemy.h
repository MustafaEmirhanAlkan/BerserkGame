#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BerserkGame/Characters/EnemyCharacters/BaseAIEnemy.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Task_AttackForEnemy.generated.h"

UCLASS(Blueprintable)
class BERSERKGAME_API UTask_AttackForEnemy : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UTask_AttackForEnemy();
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};