#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "Task_FindPlayerLocation.generated.h"

UCLASS(Blueprintable)
class BERSERKGAME_API UTask_FindPlayerLocation : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UTask_FindPlayerLocation(FObjectInitializer const& ObjectInitializer);

	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& Owner_Comp, uint8* Node_Memory) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Search", meta = (AllowProtectedAccess = "true"))
		bool Search_Random = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Search", meta = (AllowProtectedAccess = "true"))
		float Search_Radius = 500.0f;
};
