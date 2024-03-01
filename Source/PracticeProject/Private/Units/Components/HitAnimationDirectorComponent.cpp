// Copyright 2023 Vsevolod Khlebnikov. All Rights Reserved.

#include "Units/Components/HitAnimationDirectorComponent.h"

UAnimMontage* UHitAnimationDirectorComponent::GetHitAnimation(AActor* HitCauser, UCapsuleComponent* HittedActorCapsuleComponent, float TakenDamage, FAnimationsByDirection CustomAnimations)
{
	// choose hit animations data to parse
	FAnimationsByDirection SelectedHitAnimationsData;
	if ((CustomAnimations.AnimationsByDirection.Num()) > 0)
	{
		SelectedHitAnimationsData = CustomAnimations;
	}
	else
	{
		SelectedHitAnimationsData = HitAnimationsByDirection;
	}

	// find direction of hit and get animations by damage structure for this direction
	LastHitDirection = FindHitDirection(HitCauser, HittedActorCapsuleComponent);
	FAnimationsByDamage AnimationsByDamageForCurrentDirection = SelectedHitAnimationsData.AnimationsByDirection.FindRef(LastHitDirection);

	float ClampedDamageTreshold = 0.0f; // value above which, a stronger type of impact should be 

	for (const auto& Element : SelectedHitAnimationsData.HitStrengthByTakenDamage)
	{
		if (TakenDamage >= Element.Key)
		{
			ClampedDamageTreshold = Element.Key;
		}
	}

	// find hit strenght type from taken damage and then, find animation by given strenght type
	EHitStrength HitStrenghtType = SelectedHitAnimationsData.HitStrengthByTakenDamage.FindRef(ClampedDamageTreshold);
	UAnimMontage* SelectedAnimation = AnimationsByDamageForCurrentDirection.AnimationsByHitStrength.FindRef(HitStrenghtType);
	return SelectedAnimation;
}

EHitDirection UHitAnimationDirectorComponent::FindHitDirection(AActor* HitCauser, UCapsuleComponent* HittedActorCapsuleComponent)
{
	const FRotator HittedActorRotation = HittedActorCapsuleComponent->GetComponentRotation();
	const FRotator DirectionDelta = UKismetMathLibrary::FindLookAtRotation(HittedActorCapsuleComponent->GetForwardVector(), HitCauser->GetActorForwardVector());

	const float HittedActorNormalizedYaw = UKismetMathLibrary::NormalizedDeltaRotator(HittedActorRotation, DirectionDelta).Yaw;

	// clang-format off
	if (UKismetMathLibrary::InRange_FloatFloat(HittedActorNormalizedYaw, 125, 145)) { return EHitDirection::Right; }
	if (UKismetMathLibrary::InRange_FloatFloat(HittedActorNormalizedYaw, -125, 0) || UKismetMathLibrary::InRange_FloatFloat(HittedActorNormalizedYaw, 0, 125)) { return EHitDirection::Behind; }
	if (UKismetMathLibrary::InRange_FloatFloat(HittedActorNormalizedYaw, -180, -125)) { return EHitDirection::Left; }
	if (UKismetMathLibrary::InRange_FloatFloat(HittedActorNormalizedYaw, 145, 180)) { return EHitDirection::Forward; }
	else { return EHitDirection::Forward; }
	// clang-format on
}
