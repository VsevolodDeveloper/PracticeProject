// Copyright 2023 Vsevolod Khlebnikov. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "JsonSerializer/Structs/FComboAttackData.h"

#include "FUnitAttacksByWeapon.generated.h"

/**
 * Structure that holds configure of unit attacks for a certain weapon
 */
USTRUCT(BlueprintType)
struct FUnitAttacksByWeapon
{
	GENERATED_BODY()

public:
	/** Name of unit */
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Unit Attacks")
	FString UnitName = " ";

	/** Weapon that can perform certain combo attacks */
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Unit Attacks")
	FString WeaponName = " ";

	/** List of unit combo attacks for a certain weapon */
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Unit Attacks")
	TArray<FComboAttackData> ComboAttacks;
};