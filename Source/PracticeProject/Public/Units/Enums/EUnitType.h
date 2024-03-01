// Copyright 2023 Vsevolod Khlebnikov. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "EUnitType.generated.h"

/**
 * Numerator that listing all unit types
 */
UENUM(BlueprintType)
enum class EUnitType : uint8
{
    Player          UMETA(DisplayName = "Player"),
    DefaultEnemy    UMETA(DisplayName = "Default Enemy"),
	LightEnemy      UMETA(DisplayName = "Light Enemy"),
    Boss            UMETA(DisplayName = "Boss"),
};

