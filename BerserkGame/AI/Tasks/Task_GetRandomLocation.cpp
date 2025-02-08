#include "BerserkGame/AI/Tasks/Task_GetRandomLocation.h"
#include "BerserkGame/Controller/BaseEnemyController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "BerserkGame/AI/Blackboard/Blackboard_Keys.h"
#include "Kismet/GameplayStatics.h"
#include "Runtime/NavigationSystem/Public/NavigationSystem.h"

UTask_GetRandomLocation::UTask_GetRandomLocation(FObjectInitializer const& ObjectInitializer)
{
	NodeName = TEXT("Find Random Location");
}

EBTNodeResult::Type UTask_GetRandomLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	auto const cont = Cast<ABaseEnemyController>(OwnerComp.GetAIOwner());
	auto const EnemyChar = cont->GetPawn();

	FVector const Origin = EnemyChar->GetActorLocation();
	FNavLocation Location;

	UNavigationSystemV1* const Nav_Sys = UNavigationSystemV1::GetCurrent(GetWorld());
	if (Nav_Sys->GetRandomPointInNavigableRadius(Origin, Search_Radius, Location, nullptr))
	{
		cont->get_blackboard()->SetValueAsVector(Blackboard_Keys::target_location, Location.Location);
	}

	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;

}