// Copyright 2023 Vsevolod Khlebnikov. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "EWeaponType.generated.h"

/**
 * Numerator that listing all weapons certain types
 */
UENUM(BlueprintType)
enum class EWeaponType : uint8
{
    None     UMETA(DisplayName = "None"),
    Sword    UMETA(DisplayName = "Sword"),
	Mace     UMETA(DisplayName = "Mace"),
};
