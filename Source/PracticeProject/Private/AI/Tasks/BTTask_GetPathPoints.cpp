// Copyright 2023 Vsevolod Khlebnikov. All Rights Reserved.

#include "AI/Tasks/BTTask_GetPathPoints.h"
#include "AI/PatrolPath.h"
#include "AIController.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/SplineComponent.h"
#include "Units/EnemyBase.h"

UBTTask_GetPathPoints::UBTTask_GetPathPoints(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
	bCreateNodeInstance = true;
	NodeName = "Get Path Points";
	LocationIndex = 0;
}

EBTNodeResult::Type UBTTask_GetPathPoints::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const UBlackboardComponent* AIBlackboard = OwnerComp.GetBlackboardComponent();
	const AAIController* AIController = OwnerComp.GetAIOwner();

	if (!AIController || !AIBlackboard)
	{
		return EBTNodeResult::Failed;
	}

	AEnemyBase* EnemyCharacter = Cast<AEnemyBase>(AIController->GetPawn());

	if (!EnemyCharacter)
	{
		return EBTNodeResult::Failed;
	}

	APatrolPath* Path = Cast<APatrolPath>(EnemyCharacter->GetSmartObject());

	if (!Path || Path->GetPointsLocations().Num() < 1)
	{
		return EBTNodeResult::Succeeded;
	}

	OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Vector>("MoveToLocation", Path->GetPointsLocations()[LocationIndex]);

	if (LocationIndex < Path->GetPointsLocations().Num() - 1)
	{
		LocationIndex++;
		return EBTNodeResult::Succeeded;
	}

	LocationIndex = 0;
	return EBTNodeResult::Succeeded;
}
