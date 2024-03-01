// Copyright 2023 Vsevolod Khlebnikov. All Rights Reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "Components/CapsuleComponent.h"
#include "CoreMinimal.h"
#include "Kismet/KismetMathLibrary.h"
#include "Units/Structs/FAnimationsByDirection.h"

#include "HitAnimationDirectorComponent.generated.h"

/**
 * Component that choose hit animation depends on hit strenght and direction
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PRACTICEPROJECT_API UHitAnimationDirectorComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	/** Direction of the last hit */
	EHitDirection LastHitDirection;

	/** Structure that hold animation to play, choosen by specific direction */
	UPROPERTY(EditAnywhere, Category = "Animation")
	FAnimationsByDirection HitAnimationsByDirection;

	/** Find hit direction from "look at" orientation of hit causer and hitted actor */
	EHitDirection FindHitDirection(AActor* HitCauser, UCapsuleComponent* HittedActorCapsuleComponent);

public:
	/** Get hit animation by given damage and calculated direction of hit */
	UFUNCTION(BlueprintCallable, Category = "Hit Animation Director Component")
	UAnimMontage* GetHitAnimation(AActor* HitCauser, UCapsuleComponent* HittedActorCapsuleComponent, float TakenDamage, FAnimationsByDirection CustomAnimations);

	/** Get direction of the last hit */
	UFUNCTION(BlueprintPure, Category = "Hit Animation Director Component")
	EHitDirection GetLastHitDirection() const { return LastHitDirection; }
};
