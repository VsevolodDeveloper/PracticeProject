// Copyright 2023 Vsevolod Khlebnikov. All Rights Reserved.

#pragma once

#include "AIController.h"
#include "CoreMinimal.h"

#include "AIControllerBase.generated.h"

/**
 * Base class for enemy AI controller
 */
UCLASS()
class PRACTICEPROJECT_API AAIControllerBase : public AAIController
{
	GENERATED_BODY()

private:
	/** Target actor blackboard key */
	uint8 TargetActorKey;

	/** Move to target location blackboard key */
	uint8 MoveToLocationKey;

	/** Pawn controlled by this controller */
	class AEnemyBase* ControlledPawn;

protected:
	/** Behavior Tree of AIcontroller */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "AI")
	class UBehaviorTreeComponent* BT;

	/** Blackboard of AIcontroller */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "AI")
	class UBlackboardComponent* BB;

public:
	/** AAIControllerBase Default constructor */
	AAIControllerBase(const FObjectInitializer& ObjectInitializer);

	/** BeginPlay implementation */
	virtual void BeginPlay() override;

	/** OnPossess implementation */
	virtual void OnPossess(APawn* InPawn) override;
};
