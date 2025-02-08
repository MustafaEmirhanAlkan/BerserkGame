#include "BerserkGame/AI/Tasks/Task_ChasePlayer.h"
#include "BerserkGame/Controller/BaseEnemyController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "BerserkGame/AI/Blackboard/Blackboard_Keys.h"

UTask_ChasePlayer::UTask_ChasePlayer(FObjectInitializer const& ObjectInitializer)
{
	NodeName = TEXT("Chase Player");
}

EBTNodeResult::Type UTask_ChasePlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ABaseEnemyController* const cont = Cast<ABaseEnemyController>(OwnerComp.GetAIOwner());
	FVector const Player_Location = cont->get_blackboard()->GetValueAsVector(Blackboard_Keys::target_location);

	UAIBlueprintHelperLibrary::SimpleMoveToLocation(cont, Player_Location);

	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}
