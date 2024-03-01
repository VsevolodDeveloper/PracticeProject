// Copyright 2023 Vsevolod Khlebnikov. All Rights Reserved.

#pragma once

#include "Animation/AnimMontage.h"
#include "CoreMinimal.h"
#include "Units/Structs/FAnimationsByDamage.h"

#include "FAnimationsByDirection.generated.h"

/**
 * Numerator that listing the hit directions
 */
UENUM(BlueprintType)
enum class EHitDirection : uint8
{
	Forward UMETA(DisplayName = "Forward"),
	Behind UMETA(DisplayName = "Behind"),
	Right UMETA(DisplayName = "Right"),
	Left UMETA(DisplayName = "Left"),
};

/**
 * 
   Structure that holds container of FAnimationsByDamage structs for each direction,
   and configure of hit strength treshold in taken damage for each hit strength type
 */
USTRUCT(BlueprintType)
struct FAnimationsByDirection
{
	GENERATED_BODY()

public:
	/** Container of FAnimationsByDamage structs for each direction */
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	TMap<EHitDirection, FAnimationsByDamage> AnimationsByDirection;

	/** Container of hit strength treshold in taken damage for each hit strength type */
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	TMap<float, EHitStrength> HitStrengthByTakenDamage;
};