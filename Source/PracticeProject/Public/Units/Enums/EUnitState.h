// Copyright 2023 Vsevolod Khlebnikov. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "EUnitState.generated.h"

/**
 * Numerator that listing the unit states
 */
UENUM(BlueprintType)
enum class EUnitState : uint8
{
    Walking        UMETA(DisplayName = "Walking"),
    Falling        UMETA(DisplayName = "Falling"),
    ComboAttack    UMETA(DisplayName = "Combo Attack"),
    HeavyAttack    UMETA(DisplayName = "Heavy Attack"),
    Roll           UMETA(DisplayName = "Roll"),
    Dodge          UMETA(DisplayName = "Dodge"),
    Drinking       UMETA(DisplayName = "Drinking"),
    Stan           UMETA(DisplayName = "Stan"),
    Death          UMETA(DisplayName = "Death"),
};

