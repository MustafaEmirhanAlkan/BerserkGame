#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "Perception/AIPerceptionComponent.h"
#include "BaseEnemyController.generated.h"

UCLASS()
class BERSERKGAME_API ABaseEnemyController : public AAIController
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere)
		UAIPerceptionComponent* AIPerceptionComponent;

	UPROPERTY(VisibleAnywhere)
		UBlackboardComponent* EnemyBlackboard;

	virtual void BeginPlay() override;

public:
	ABaseEnemyController(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable)
		void StartAttackAnimation();

	UPROPERTY(BlueprintReadWrite)
		class ABaseAIEnemy* Enemy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UBehaviorTree* BehaviorTree;

	UFUNCTION()
		void OnPerception(AActor* Actor, FAIStimulus Stimulus);

	bool ShouldAttack(AActor* TargetActor);

	UFUNCTION()
		void SetCanSeePlayer(bool SeePlayer, class UObject* Player);

	UFUNCTION()
		void RandomPatrol();

	FTimerHandle RetriggerableTimerHandle;
	FTimerDelegate FunctionDelegate;
	void RunRetriggerableTimer();
	class UAISenseConfig_Sight* Sight;

	virtual void OnPossess(APawn* InPawn) override;

	UBlackboardComponent* get_blackboard() const { return EnemyBlackboard; }

private:
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = "true"))
		class UBehaviorTreeComponent* BehaviorTree_Component;

	class UAISenseConfig_Sight* Sight_Config;

	class UAISenseConfig_Hearing* Hearing_Config;

	void On_Target_Detected(AActor* actor, FAIStimulus const stimulus);

	void On_Updated(TArray<AActor*> const& Update_Actors);

	void Setup_perception_system();

	class UNavigationSystemV1* NavArea;

	FVector RandomLocation;
};
