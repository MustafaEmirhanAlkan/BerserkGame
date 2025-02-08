#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Task_StopInvestigating.generated.h"

UCLASS(Blueprintable)
class BERSERKGAME_API UTask_StopInvestigating : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UTask_StopInvestigating(FObjectInitializer const& ObjectInitializer);

	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};