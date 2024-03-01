// Copyright 2023 Vsevolod Khlebnikov. All Rights Reserved.

#pragma once

#include "Animation/AnimMontage.h"
#include "CoreMinimal.h"

#include "FAnimationsByDamage.generated.h"

/**
 * Numerator that listing the hit strength types
 */
UENUM(BlueprintType)
enum class EHitStrength : uint8
{
	NoReaction UMETA(DisplayName = "No Reaction"),
	LightHit UMETA(DisplayName = "Light Hit"),
	MediumHit UMETA(DisplayName = "Medium Hit"),
	HeavyHit UMETA(DisplayName = "Heavy Hit"),
	Knockdown UMETA(DisplayName = "Knockdown"),
};

/**
 * Structure that holds container of hit animations for each hit strength type
 */
USTRUCT(BlueprintType)
struct FAnimationsByDamage
{
	GENERATED_BODY()

public:
	/** Container of animations for each hit strength type */
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	TMap<EHitStrength, UAnimMontage*> AnimationsByHitStrength;
};