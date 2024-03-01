// Copyright 2023 Vsevolod Khlebnikov. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "FComboAttackData.generated.h"

/**
 * Structure that holds data about combo attack chain
 */
USTRUCT(BlueprintType)
struct FComboAttackData
{
	GENERATED_BODY()

public:
	/** Id of combo attack chain */
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Combo Attack Data")
	FString ComboName = " ";

	/** Required stamina for performing this attack chain */
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Combo Attack Data")
	int32 RequiredStamina = 0;

	/** chain of attacks in combo */
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Combo Attack Data")
	TArray<FString> AttacksChain;

	/** Damage multiplier for all attacks in the current combo */
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Combo Attack Data")
	float DamageRate = 1.0f;
};