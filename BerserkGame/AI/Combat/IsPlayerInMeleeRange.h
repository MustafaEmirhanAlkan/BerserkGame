#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "IsPlayerInMeleeRange.generated.h"

UCLASS()
class BERSERKGAME_API UIsPlayerInMeleeRange : public UBTService_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UIsPlayerInMeleeRange();
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = "true"))
		float Melee_Range = 15.0f;
};