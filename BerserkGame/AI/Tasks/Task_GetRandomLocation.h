#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "GameFramework/Pawn.h"
#include "UObject/UObjectGlobals.h"
#include "Task_GetRandomLocation.generated.h"

UCLASS(Blueprintable)
class BERSERKGAME_API UTask_GetRandomLocation : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UTask_GetRandomLocation(FObjectInitializer const& ObjectInitializer);
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Search", meta = (AllowPrivateAccess = "true"))
		float Search_Radius = 1500.0f;
};
