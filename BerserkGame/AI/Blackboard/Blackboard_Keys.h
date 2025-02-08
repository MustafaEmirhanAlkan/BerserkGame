#pragma once
#include "Runtime/Core/Public/UObject/NameTypes.h"
#include "Runtime/Core/Public/Containers/UnrealString.h"
#include "BerserkGame/AI/Tasks/Task_AttackForEnemy.h"

namespace Blackboard_Keys
{
	TCHAR const* const target_location = TEXT("TargetLocation");
	TCHAR const* const can_see_player = TEXT("CanSeePlayer");
	TCHAR const* const player_is_in_melee_range = TEXT("PlayerIsInMeleeRange");
	TCHAR const* const player_is_in_projectile_range = TEXT("PlayerIsInProjectileRange");
	TCHAR const* const is_investigating = TEXT("IsInvestigating");
	TCHAR const* const player_target = TEXT("PlayerTarget");
	TCHAR const* const can_melee_attack = TEXT("CanEverMeleeAttack");
	TCHAR const* const can_ranged_attack = TEXT("CanEverRangedAttack");
}