// Copyright 2023 Vsevolod Khlebnikov. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "FAttackAnimations.generated.h"

/**
 * Structure that holds container of hit animations for each hit strength type
 */
USTRUCT(BlueprintType)
struct FAnimationStructure
{
	GENERATED_BODY()

public:
	/** Animation of heavy attack */
	UPROPERTY(EditAnywhere, Category = "Animation")
	UAnimMontage* HeavyAttackAnimation;

	/** Container of animations for each attack in combo */
	UPROPERTY(EditAnywhere, Category = "Animation")
	TMap<FString, UAnimMontage*> ComboAttackAnimationMoveset;
};