#include "BerserkGame/AI/Tasks/Task_AttackForEnemy.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"
#include "BerserkGame/Controller/BaseEnemyController.h"
#include "BerserkGame/Characters/EnemyCharacters/BaseAIEnemy.h"
#include "BerserkGame/Characters/EnemyCharacters/MeleeEnemy.h"
#include "BerserkGame/Characters/EnemyCharacters/RangedEnemy.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BerserkGame/AI/Blackboard/Blackboard_Keys.h"
#include "BerserkGame/Characters/MainCharacter/BaseMainCharacter.h"
#include "BerserkGame/AI/Combat/CombatInterface.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/LatentActionManager.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "BerserkGame/Characters/MainCharacter/MainCharAnimInstance.h"

UTask_AttackForEnemy::UTask_AttackForEnemy()
{
	NodeName = TEXT("Attack For Enemy");
}

EBTNodeResult::Type UTask_AttackForEnemy::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UE_LOG(LogTemp, Warning, TEXT("UTask_AttackForEnemy"));

	ABaseEnemyController* const EnemyController = Cast<ABaseEnemyController>(OwnerComp.GetAIOwner());
	if (!EnemyController)
	{
		UE_LOG(LogTemp, Warning, TEXT("Faild"));

		return EBTNodeResult::Failed;
	}

	ABaseAIEnemy* const EnemyPawn = Cast<ABaseAIEnemy>(EnemyController->GetPawn());
	if (!EnemyPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("Faild"));

		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* const BlackboardComp = EnemyController->GetBlackboardComponent();
	if (!BlackboardComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("Faild"));

		return EBTNodeResult::Failed;
	}

	const bool bCanMeleeAttack = BlackboardComp->GetValueAsBool(Blackboard_Keys::can_melee_attack);
	const bool bCanRangedAttack = BlackboardComp->GetValueAsBool(Blackboard_Keys::can_ranged_attack);

	if (bCanMeleeAttack)
	{
		UE_LOG(LogTemp, Warning, TEXT("bCanMeleeAttack"));

		EnemyPawn->MeleeAttack();
	}
	else if (bCanRangedAttack)
	{
		UE_LOG(LogTemp, Warning, TEXT("bCanRangedAttack"));

		EnemyPawn->RangedAttack();
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("else"));

	return EBTNodeResult::Succeeded;
}