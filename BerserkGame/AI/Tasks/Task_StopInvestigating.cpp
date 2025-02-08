#include "BerserkGame/AI/Tasks/Task_StopInvestigating.h"
#include "BerserkGame/Controller/BaseEnemyController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BerserkGame/AI/Blackboard/Blackboard_Keys.h"

UTask_StopInvestigating::UTask_StopInvestigating(FObjectInitializer const& ObjectInitializer)
{
	NodeName = TEXT("Stop Investigating");
}

EBTNodeResult::Type UTask_StopInvestigating::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	auto const cont = Cast<ABaseEnemyController>(OwnerComp.GetAIOwner());
	cont->get_blackboard()->SetValueAsBool(Blackboard_Keys::is_investigating, false);

	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}
