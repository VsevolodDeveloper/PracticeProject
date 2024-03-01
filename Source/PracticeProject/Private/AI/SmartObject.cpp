// Copyright 2023 Vsevolod Khlebnikov. All Rights Reserved.

#include "AI/SmartObject.h"
#include "Components/ArrowComponent.h"
#include "Components/BillboardComponent.h"

// Sets default values
ASmartObject::ASmartObject()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Billboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("Billboard"));
	Billboard->SetupAttachment(GetRootComponent());

	FacingDirection = CreateDefaultSubobject<UArrowComponent>(TEXT("Facing Direction"));
	FacingDirection->SetupAttachment(Billboard);
}
