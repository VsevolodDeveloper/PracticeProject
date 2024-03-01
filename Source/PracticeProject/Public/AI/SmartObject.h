// Copyright 2023 Vsevolod Khlebnikov. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "SmartObject.generated.h"

/**
 * Actor that provide to AIController some data
 */
UCLASS()
class PRACTICEPROJECT_API ASmartObject : public AActor
{
	GENERATED_BODY()

private:
	/** Component for visual assist */
	UPROPERTY(EditAnywhere, Category = "Components")
	class UBillboardComponent* Billboard;

	/** Component that show direction of AI */
	UPROPERTY(EditAnywhere, Category = "Components")
	class UArrowComponent* FacingDirection;

public:
	/** Pointer to pawn behavior tree */
	UPROPERTY(EditAnywhere, Category = "AI")
	class UBehaviorTree* PawnBT;

public:
	// Sets default values for this actor's properties
	ASmartObject();
};
