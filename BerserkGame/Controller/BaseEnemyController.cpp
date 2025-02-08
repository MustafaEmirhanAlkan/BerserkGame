#include "BerserkGame/Controller/BaseEnemyController.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BerserkGame/Characters/EnemyCharacters/BaseAIEnemy.h"
#include "BerserkGame/Characters/EnemyCharacters/MeleeEnemy.h"
#include "BerserkGame/Characters/EnemyCharacters/RangedEnemy.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BerserkGame/AI/Blackboard/Blackboard_Keys.h"
#include "BerserkGame/AI/Blackboard/Enemy_Tags.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"
#include "NavigationSystem.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"
#include "Runtime/AIModule/Classes/Perception/AISenseConfig_Sight.h"
#include "UObject/ConstructorHelpers.h"


ABaseEnemyController::ABaseEnemyController(const FObjectInitializer& ObjectInitializer)
{
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComp"));

	Sight = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	Sight->SightRadius = 1000.0f;
	Sight->LoseSightRadius = Sight->SightRadius + 500.0f;
	Sight->PeripheralVisionAngleDegrees = 60.0f;
	Sight->DetectionByAffiliation.bDetectNeutrals = true;

	AIPerceptionComponent->ConfigureSense(*Sight);
	AIPerceptionComponent->SetDominantSense(Sight->GetSenseImplementation());

	BehaviorTree_Component = ObjectInitializer.CreateDefaultSubobject<UBehaviorTreeComponent>(this, TEXT("BehaviorComp"));
	EnemyBlackboard = ObjectInitializer.CreateDefaultSubobject<UBlackboardComponent>(this, TEXT("BlackboardComp"));
}

void ABaseEnemyController::BeginPlay()
{
	Super::BeginPlay();

	RunBehaviorTree(BehaviorTree);
	BehaviorTree_Component->StartTree(*BehaviorTree);

	NavArea = FNavigationSystem::GetCurrent<UNavigationSystemV1>(this);

	RandomPatrol();

	ABaseAIEnemy* Chr = Cast<ABaseAIEnemy>(GetPawn());
	if (Chr)
	{
		Enemy = Chr;
	}
}

void ABaseEnemyController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);


	if (BehaviorTree)
	{
		RunBehaviorTree(BehaviorTree);
		UBlackboardComponent* BlackboardComp = GetBlackboardComponent();

		if (AIPerceptionComponent)
		{
			AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ABaseEnemyController::OnPerception);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("AIPerceptionComponent is not initialized."));
		}

		ARangedEnemy* RangedEnemy = Cast<ARangedEnemy>(InPawn);
		AMeleeEnemy* MeleeEnemy = Cast<AMeleeEnemy>(InPawn);
		ABaseAIEnemy* BasePawn = Cast<ABaseAIEnemy>(InPawn);

		if (BasePawn)
		{
			BlackboardComp->SetValueAsBool(Blackboard_Keys::can_melee_attack, BasePawn->CanEverMeleeAttack);
			BlackboardComp->SetValueAsBool(Blackboard_Keys::can_ranged_attack, BasePawn->CanEverRangedAttack);
		}
	}
}

void ABaseEnemyController::OnPerception(AActor* Actor, FAIStimulus Stimulus)
{
	ABaseMainCharacter* Chr = Cast<ABaseMainCharacter>(Actor);
	if (Chr == nullptr)
	{
		return;
	}

	if (Stimulus.Type == UAISense::GetSenseID<UAISense_Sight>())
	{
		if (Stimulus.WasSuccessfullySensed())
		{
			SetCanSeePlayer(true, Actor);

			FVector EnemyLocation = GetPawn()->GetActorLocation();
			FVector PlayerLocation = Actor->GetActorLocation();
			float DistanceToPlayer = FVector::Distance(EnemyLocation, PlayerLocation);

			if (DistanceToPlayer < 50.0f)
			{
				StartAttackAnimation();
			}
		}
		else
		{
			SetCanSeePlayer(false, Actor);
		}
	}
	//Enemy->SetAnimState(Stimulus.WasSuccessfullySensed());
	SetFocus(Stimulus.WasSuccessfullySensed() ? Chr : nullptr);
}

bool ABaseEnemyController::ShouldAttack(AActor* TargetActor)
{
	if (TargetActor && TargetActor->IsValidLowLevel())
	{
		float Distance = FVector::Distance(GetPawn()->GetActorLocation(), TargetActor->GetActorLocation());

		if (Distance < 50.0f)
		{
			return true;
		}
	}
	return false;
}

void ABaseEnemyController::StartAttackAnimation()
{
	//if (Enemy && Enemy->GetMesh() && Enemy->Sword_And_Shield_Slash)
	//{
	//	UAnimInstance* AnimInstance = Enemy->GetMesh()->GetAnimInstance();
	//	if (AnimInstance)
	//	{
	//		AnimInstance->Montage_Play(Enemy->Sword_And_Shield_Slash, 1.0f);
	//	}
	//}

	//if (Enemy && Enemy->GetMesh() && Enemy->Standing_1H_Magic_Attack_01)
	//{
	//	UAnimInstance* AnimInstance = Enemy->GetMesh()->GetAnimInstance();
	//	if (AnimInstance)
	//	{
	//		AnimInstance->Montage_Play(Enemy->Standing_1H_Magic_Attack_01, 1.0f);
	//	}
	//}
}

void ABaseEnemyController::SetCanSeePlayer(bool SeePlayer, UObject* Player)
{
	GetBlackboardComponent()->SetValueAsBool(Blackboard_Keys::can_see_player, SeePlayer);

	if (SeePlayer)
	{
		GetBlackboardComponent()->SetValueAsObject(Blackboard_Keys::player_target, Player);
	}
	else
	{

		ACharacter* EnemyChar = Cast<ACharacter>(GetPawn());
		EnemyChar->GetMesh()->GetAnimInstance()->StopAllMontages(0);
	}
}

void ABaseEnemyController::RunRetriggerableTimer()
{
}

void ABaseEnemyController::On_Target_Detected(AActor* actor, FAIStimulus const stimulus)
{
	if (auto const ch = Cast<ABaseAIEnemy>(actor))
	{
		get_blackboard()->SetValueAsBool(Blackboard_Keys::can_see_player, stimulus.WasSuccessfullySensed());
	}
}

void ABaseEnemyController::On_Updated(TArray<AActor*> const& Update_Actors)
{
	for (size_t x = 0; x < Update_Actors.Num(); ++x)
	{
		FActorPerceptionBlueprintInfo info;
		GetPerceptionComponent()->GetActorsPerception(Update_Actors[x], info);
		for (size_t k = 0; k < info.LastSensedStimuli.Num(); ++k)
		{
			FAIStimulus const Stim = info.LastSensedStimuli[k];
			//if (Stim.Tag == Tags::noise_tag)
			//{
			//	get_blackboard()->SetValueAsBool(Blackboard_Keys::is_investigating, Stim.WasSuccessfullySensed());
			//	get_blackboard()->SetValueAsVector(Blackboard_Keys::target_location, Stim.StimulusLocation);
			//}
			//else
			//{
			//	get_blackboard()->SetValueAsBool(Blackboard_Keys::can_see_player, Stim.WasSuccessfullySensed());
			//}
		}

	}
}

void ABaseEnemyController::Setup_perception_system()
{
	Sight_Config = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	if (Sight_Config)
	{
		SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception Component")));
		Sight_Config->SightRadius = 500.0f;
		Sight_Config->LoseSightRadius = Sight_Config->SightRadius + 50.0f;
		Sight_Config->PeripheralVisionAngleDegrees = 60.0f;
		Sight_Config->SetMaxAge(5.0f);
		Sight_Config->AutoSuccessRangeFromLastSeenLocation = 900.0f;
		Sight_Config->DetectionByAffiliation.bDetectEnemies =
		Sight_Config->DetectionByAffiliation.bDetectFriendlies =
		Sight_Config->DetectionByAffiliation.bDetectNeutrals = true;

		GetPerceptionComponent()->SetDominantSense(*Sight_Config->GetSenseImplementation());
		GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &ABaseEnemyController::On_Target_Detected);
		GetPerceptionComponent()->ConfigureSense(*Sight_Config);
	}

	Hearing_Config = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("Hearing Config"));
	if (Hearing_Config)
	{
		Hearing_Config->HearingRange = 2000.0f;
		Hearing_Config->DetectionByAffiliation.bDetectEnemies =
		Hearing_Config->DetectionByAffiliation.bDetectFriendlies =
		Hearing_Config->DetectionByAffiliation.bDetectNeutrals = true;

		GetPerceptionComponent()->OnPerceptionUpdated.AddDynamic(this, &ABaseEnemyController::On_Updated);
		GetPerceptionComponent()->ConfigureSense(*Hearing_Config);
	}
}

void ABaseEnemyController::RandomPatrol()
{
	if (NavArea)
	{
		NavArea->K2_GetRandomReachablePointInRadius(GetWorld(), GetPawn()->GetActorLocation(), RandomLocation, 15000.0f);
		MoveToLocation(RandomLocation);
	}
}
