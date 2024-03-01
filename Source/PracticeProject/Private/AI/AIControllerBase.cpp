// Copyright 2023 Vsevolod Khlebnikov. All Rights Reserved.

#include "AI/AIControllerBase.h"
#include "AI/SmartObject.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Units/EnemyBase.h"

AAIControllerBase::AAIControllerBase(const FObjectInitializer& ObjectInitializer)
{
	BB = CreateDefaultSubobject<UBlackboardComponent>(TEXT("Blackboard Component"));
	BT = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTree Component"));
}

void AAIControllerBase::BeginPlay()
{
	Super::BeginPlay();

	if (!ControlledPawn)
	{
		AEnemyBase* EnemyCharacter = Cast<AEnemyBase>(GetPawn());
		if (!EnemyCharacter)
		{
			return;
		}
		ControlledPawn = EnemyCharacter;
	}

	if (ControlledPawn->GetSmartObject())
	{
		const FGameplayTag SubTag;
		BT->SetDynamicSubtree(SubTag, ControlledPawn->GetBehaviorTree());
	}
}

void AAIControllerBase::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	AEnemyBase* EnemyCharacter = Cast<AEnemyBase>(InPawn);
	if (EnemyCharacter != nullptr && EnemyCharacter->GetBehaviorTree() != nullptr)
	{
		ControlledPawn = EnemyCharacter;

		BB->InitializeBlackboard(*EnemyCharacter->GetBehaviorTree()->BlackboardAsset);

		TargetActorKey = BB->GetKeyID("TargetActor");
		MoveToLocationKey = BB->GetKeyID("MoveToLocation");

		BT->StartTree(*EnemyCharacter->GetBehaviorTree());
	}
}
