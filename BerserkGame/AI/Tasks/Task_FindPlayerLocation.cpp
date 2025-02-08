#include "BerserkGame/AI/Tasks/Task_FindPlayerLocation.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BerserkGame/Controller/BaseEnemyController.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "BerserkGame/AI/Blackboard/Blackboard_Keys.h"
#include "GameFramework/Character.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/NavigationSystem/Public/NavigationSystem.h"

UTask_FindPlayerLocation::UTask_FindPlayerLocation(FObjectInitializer const& ObjectInitializer)
{
	NodeName = TEXT("Find Player Location");
}

EBTNodeResult::Type UTask_FindPlayerLocation::ExecuteTask(UBehaviorTreeComponent& Owner_Comp, uint8* Node_Memory)
{
	ACharacter* const player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	auto const cont = Cast<ABaseEnemyController>(Owner_Comp.GetAIOwner());

	FVector const Player_Location = player->GetActorLocation();
	if (Search_Random)
	{
		FNavLocation loc;

		UNavigationSystemV1* const Nav_Sys = UNavigationSystemV1::GetCurrent(GetWorld());
		if (Nav_Sys->GetRandomPointInNavigableRadius(Player_Location, Search_Radius, loc, nullptr))
		{
			cont->get_blackboard()->SetValueAsVector(Blackboard_Keys::target_location, loc.Location);
		}
	}
	else
	{
		cont->get_blackboard()->SetValueAsVector(Blackboard_Keys::target_location, Player_Location);
	}
	return EBTNodeResult::Succeeded;
}
