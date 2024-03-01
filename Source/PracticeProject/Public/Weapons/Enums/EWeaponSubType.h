// Copyright 2023 Vsevolod Khlebnikov. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "EWeaponSubType.generated.h"

/**
 * Numerator that listing weapons sub-types
 */
UENUM(BlueprintType)
enum class EWeaponSubType : uint8
{
    None          UMETA(DisplayName = "None"),
    OneHanded     UMETA(DisplayName = "One-Handed"),
    TwoHanded     UMETA(DisplayName = "Two-Handed"),
};
