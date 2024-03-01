// Copyright 2023 Vsevolod Khlebnikov. All Rights Reserved.

#pragma once

#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "CoreMinimal.h"
#include "UObject/UObjectGlobals.h"

#include "BTTask_GetPathPoints.generated.h"

/**
 * Get Path points for patrolling task
 */
UCLASS()
class PRACTICEPROJECT_API UBTTask_GetPathPoints : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

private:
	/** Path point location index */
	int32 LocationIndex;

public:
	/** GetPathPoints Default constructor */
	UBTTask_GetPathPoints(const FObjectInitializer& ObjectInitializer);

	/** ExecuteTask Implementation */
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
