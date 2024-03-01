// Copyright 2023 Vsevolod Khlebnikov. All Rights Reserved.

#include "SimpleFootIK_FunctionLibrary.h"

void USimpleFootIK_FunctionLibrary::CalculateFootOffset(float& CurrentFootOffsetAlpha, float FootDistance, float DeltaTime, float InterpSpeed, const FVector& FootNormal, FRotator& FootRotation)
{
	const float NewFootOffsetAlpha = (FootDistance - 110) / -45;
	CurrentFootOffsetAlpha = FMath::Clamp(CurrentFootOffsetAlpha, -1.0f, 1.0f);
	CurrentFootOffsetAlpha = FMath::FInterpTo(CurrentFootOffsetAlpha, NewFootOffsetAlpha, DeltaTime, InterpSpeed);
	FootRotation = FootRotation + (FRotator(-180.0f / PI * FGenericPlatformMath::Atan2(FootNormal.X, FootNormal.Z), 0.0f, 180.0f / PI * FGenericPlatformMath::Atan2(FootNormal.Y, FootNormal.Z)) - FootRotation);
}

void USimpleFootIK_FunctionLibrary::CalculateRootDisplacement(float& CurrentRootDisplacement, float LeftFootDistance, float RightFootDistance, float DeltaTime, float InterpSpeed, float CapsuleHalfHeight)
{
	const float FootDistance = LeftFootDistance >= RightFootDistance ? LeftFootDistance : RightFootDistance;
	const float NewRootDiplacement = (FootDistance - CapsuleHalfHeight) * -1;
	CurrentRootDisplacement = FMath::FInterpTo(CurrentRootDisplacement, NewRootDiplacement, DeltaTime, InterpSpeed);
}

float USimpleFootIK_FunctionLibrary::GetRootDisplacement(const FName& SocketName, USkeletalMeshComponent* Mesh, float CapsuleHalfHeight, const FVector& RootDiplacementTraceOffset, bool bComplexTrace, bool& bBlockingHit)
{
	const float MeshHalfHeight = (Mesh->GetComponentLocation() + FVector(0.0f, 0.0f, CapsuleHalfHeight)).Z;
	const FVector TraceStartPoint = FVector(Mesh->GetSocketLocation(SocketName).X, Mesh->GetSocketLocation(SocketName).Y, MeshHalfHeight);
	const FVector TraceEndPoint = TraceStartPoint - RootDiplacementTraceOffset;
	FHitResult OutHit;

	UKismetSystemLibrary::LineTraceSingle(Mesh->GetWorld(), TraceStartPoint, TraceEndPoint, ETraceTypeQuery::TraceTypeQuery1, bComplexTrace, {}, EDrawDebugTrace::None, OutHit, true, FColor::Red, FColor::Green, 0.1f);
	bBlockingHit = OutHit.bBlockingHit;
	return OutHit.Distance;
}

FHitResult USimpleFootIK_FunctionLibrary::CalculateFootPosition(const FName& SocketName, USkeletalMeshComponent* Mesh, const FVector& FootPositionOffset, bool bComplexTrace, bool& bHit)
{
	FVector NewFootPositionOffset = Mesh->GetSocketLocation(SocketName);
	NewFootPositionOffset.Z = Mesh->GetSocketLocation("root").Z;

	const FVector TraceStartPoint = NewFootPositionOffset + FootPositionOffset;
	const FVector TraceEndPoint = NewFootPositionOffset - FootPositionOffset;
	FHitResult OutHit;

	bHit = UKismetSystemLibrary::LineTraceSingle(Mesh->GetWorld(), TraceStartPoint, TraceEndPoint, ETraceTypeQuery::TraceTypeQuery1, bComplexTrace, {}, EDrawDebugTrace::None, OutHit, true, FColor::Red, FColor::Green, 0.1f);
	return OutHit;
}

void USimpleFootIK_FunctionLibrary::CalculateFootIK(FSimpleFootIKData& FootIKData, USkeletalMeshComponent* Mesh, float DeltaTime, float InterpSpeed, bool bComplexTrace)
{
	bool RFD_IsHit; //Is Right Foot Displacement Hit
	const float RightFootDisplacement = GetRootDisplacement(FootIKData.RightFootName, Mesh, FootIKData.TraceLineHeight, FootIKData.IK_Offsets.RootDiplacementTraceOffset, bComplexTrace, RFD_IsHit);

	bool LFD_IsHit; //Is Left Foot Displacement Hit
	const float LeftFootDisplacement = GetRootDisplacement(FootIKData.LeftFootName, Mesh, FootIKData.TraceLineHeight, FootIKData.IK_Offsets.RootDiplacementTraceOffset, bComplexTrace, LFD_IsHit);

	if (RFD_IsHit || LFD_IsHit)
	{
		CalculateRootDisplacement(FootIKData.RootDisplacementOffset, LeftFootDisplacement, RightFootDisplacement, DeltaTime, InterpSpeed, FootIKData.TraceLineHeight);

		bool RF_IsHit; //Is Right Foot Hit
		FHitResult RF_Hit = CalculateFootPosition(FootIKData.RightFootName, Mesh, FootIKData.IK_Offsets.RightFootPositionTraceOffset, bComplexTrace, RF_IsHit);
		CalculateFootOffset(FootIKData.RightFootOffsetAlpha, RF_IsHit ? RF_Hit.Distance : BIG_NUMBER, DeltaTime, InterpSpeed, RF_Hit.Normal, FootIKData.RightFootRotation);

		bool LF_IsHit; // Is Left Foot Hit
		FHitResult LF_Hit = CalculateFootPosition(FootIKData.LeftFootName, Mesh, FootIKData.IK_Offsets.LeftFootPositionTraceOffset, bComplexTrace, LF_IsHit);
		CalculateFootOffset(FootIKData.LeftFootOffsetAlpha, LF_IsHit ? LF_Hit.Distance : BIG_NUMBER, DeltaTime, InterpSpeed, LF_Hit.Normal, FootIKData.LeftFootRotation);
	}
}

void USimpleFootIK_FunctionLibrary::SimpleFootIK(FSimpleFootIKData& FootIKData, bool IsInAir, USkeletalMeshComponent* Mesh, float DeltaTime, float InterpSpeed, bool bComplexTrace)
{
	if (!IsInAir)
	{
		CalculateFootIK(FootIKData, Mesh, DeltaTime, InterpSpeed, bComplexTrace);
	}
	else
	{
		FootIKData.LeftFootOffsetAlpha = 0.0f;
		FootIKData.RightFootOffsetAlpha = 0.0f;
		FootIKData.RootDisplacementOffset = 0.0f;
	}
}

void USimpleFootIK_FunctionLibrary::InitializeFootIK(FSimpleFootIKData& FootIK_Data, UAnimInstance* AnimInstance)
{
	if (APawn* Pawn = AnimInstance->TryGetPawnOwner())
	{
		if (ACharacter* Character = Cast<ACharacter>(Pawn))
		{
			FootIK_Data.TraceLineHeight = Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
			FootIK_Data.RightFootName = FootIK_Data.RightFoot.BoneName;
			FootIK_Data.LeftFootName = FootIK_Data.LeftFoot.BoneName;
			if (FootIK_Data.RightFoot.BoneName != "None" && FootIK_Data.LeftFoot.BoneName != "None")
			{
				FootIK_Data.bIsInitialized = true;
			}
		}
	}
}

void USimpleFootIK_FunctionLibrary::UpdateFootIKData(UAnimInstance* AnimInstance, UPARAM(ref) FSimpleFootIKData& FootIK_Data, UPARAM(ref) FSimpleFootIKOffsets& Offsets, float MaxCharacterSpeedToUpdate, float InterpSpeed, bool bComplexTrace)
{
	if (!FootIK_Data.bIsInitialized)
	{
		InitializeFootIK(FootIK_Data, AnimInstance);
	}
	if (APawn* Pawn = AnimInstance->TryGetPawnOwner())
	{
		if (ACharacter* Character = Cast<ACharacter>(Pawn))
		{
			if (UKismetMathLibrary::VSize(Character->GetVelocity()) > MaxCharacterSpeedToUpdate)
			{
				return;
			}
			const float DeltaTime = Character->GetWorld()->DeltaTimeSeconds;
			FootIK_Data.IK_Offsets = Offsets;
			SimpleFootIK(FootIK_Data, Character->GetMovementComponent()->IsFalling(), Character->GetMesh(), DeltaTime, InterpSpeed, bComplexTrace);
		}
	}
}
