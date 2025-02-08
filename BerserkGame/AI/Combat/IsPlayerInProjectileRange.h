#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "IsPlayerInProjectileRange.generated.h"

UCLASS()
class BERSERKGAME_API UIsPlayerInProjectileRange : public UBTService_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UIsPlayerInProjectileRange();
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = "true"))
		float Projectile_Range = 225.0f;
};
