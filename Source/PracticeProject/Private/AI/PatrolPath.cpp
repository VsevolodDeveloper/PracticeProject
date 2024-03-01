// Copyright 2023 Vsevolod Khlebnikov. All Rights Reserved.

#include "AI/PatrolPath.h"
#include "Components/SplineComponent.h"

APatrolPath::APatrolPath()
{
	PrimaryActorTick.bCanEverTick = false;

	Path = CreateDefaultSubobject<USplineComponent>(TEXT("Path"));
	Path->SetupAttachment(RootComponent);
	Path->bDrawDebug = true;
}

void APatrolPath::BeginPlay()
{
	Super::BeginPlay();
	SetSplinePoints();
}

void APatrolPath::SetSplinePoints()
{
	for (int i = 0; i <= Path->GetNumberOfSplinePoints(); ++i)
	{
		PointsLocations.Add(Path->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World));
	}
}
