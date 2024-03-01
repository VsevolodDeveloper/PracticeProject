// Copyright 2023 Vsevolod Khlebnikov. All Rights Reserved.

#pragma once

#include "AI/SmartObject.h"
#include "CoreMinimal.h"

#include "PatrolPath.generated.h"

/**
 * Object that help AI found patrolling path
 */
UCLASS()
class PRACTICEPROJECT_API APatrolPath : public ASmartObject
{
	GENERATED_BODY()

private:
	/** Patrolling path */
	UPROPERTY(VisibleAnyWhere, Category = "Spline Path");
	class USplineComponent* Path;

	/** Array of points locations */
	TArray<FVector> PointsLocations;

	/** Set array of points locations */
	void SetSplinePoints();

public:
	/** APatrolPath default constructor */
	APatrolPath();

	/** Get array of points locations */
	TArray<FVector> GetPointsLocations() { return PointsLocations; }

	/** BeginPlay implementation */
	virtual void BeginPlay() override;
};
