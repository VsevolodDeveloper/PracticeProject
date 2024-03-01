// Copyright 2023 Vsevolod Khlebnikov. All Rights Reserved.

#pragma once

#include "Components/CapsuleComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

#include "SimpleFootIK_FunctionLibrary.generated.h"

/**
 * Structure of foot position offsets
 */
USTRUCT(BlueprintType, meta = (DisplayName = "Simple Foot IK Offsets"))
struct FSimpleFootIKOffsets
{
	GENERATED_BODY()

public:
	/* Offset of Root Diplacement */
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "IK Offsets")
	FVector RootDiplacementTraceOffset = FVector(0.0f, 0.0f, 150.0f);

	/* Offset of Right Foot Position */
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "IK Offsets")
	FVector RightFootPositionTraceOffset = FVector(0.0f, 0.0f, 95.0f);

	/* Offset of Left Foot Position */
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "IK Offsets")
	FVector LeftFootPositionTraceOffset = FVector(0.0f, 0.0f, 95.0f);
};

/**
 * Structure of SFIK data
 */
USTRUCT(BlueprintType, meta = (DisplayName = "Simple Foot IK Data"))
struct FSimpleFootIKData
{
	GENERATED_BODY()

public:
	/* Right foot bone */
	UPROPERTY(EditAnyWhere, Category = "Bones")
	FBoneReference RightFoot;

	/* Left foot bone */
	UPROPERTY(EditAnyWhere, Category = "Bones")
	FBoneReference LeftFoot;

	/* Root bone */
	UPROPERTY(EditAnyWhere, Category = "Bones")
	FBoneReference Root;

	/* Right foot bone name */
	FName RightFootName;

	/* Left foot bone name */
	FName LeftFootName;

	/* Right foot offset value */
	UPROPERTY(BlueprintReadOnly)
	float RightFootOffsetAlpha;

	/* Left foot offset value */
	UPROPERTY(BlueprintReadOnly)
	float LeftFootOffsetAlpha;

	/* Root displacement value */
	float RootDisplacementOffset;

	/* Trace line height value */
	float TraceLineHeight;

	/* Left foot rotation value */
	FRotator LeftFootRotation;

	/* Right foot rotation value */
	FRotator RightFootRotation;

	/* Structure of foot position offsets */
	FSimpleFootIKOffsets IK_Offsets;

	/* Is data already initialized or not */
	bool bIsInitialized;
};

/**
 * Function Library for SFIK node
 */
UCLASS()
class SIMPLEFOOTIK_API USimpleFootIK_FunctionLibrary : public UObject
{
	GENERATED_BODY()

private:
	/* Calculate foot offset and rotation */
	static void CalculateFootOffset(float& CurrentFootOffsetAlpha, float FootDistance, float DeltaTime, float InterpSpeed, const FVector& FootNormal, FRotator& FootRotation);

	/* Calculate root displacement depending on far foot */
	static void CalculateRootDisplacement(float& CurrentRootDisplacement, float LeftFootDistance, float RightFootDistance, float DeltaTime, float InterpSpeed, float CapsuleHalfHeight);

	/* Calculate root displacement for every foot */
	static float GetRootDisplacement(const FName& SocketName, USkeletalMeshComponent* Mesh, float CapsuleHalfHeight, const FVector& RootDiplacementTraceOffset, bool bComplexTrace, bool& bBlockingHit);

	/* Calculate foot position and set it */
	static FHitResult CalculateFootPosition(const FName& SocketName, USkeletalMeshComponent* Mesh, const FVector& FootPositionOffset, bool bComplexTrace, bool& bHit);

	/* Calculate and return all FootIK data */
	static void CalculateFootIK(FSimpleFootIKData& FootIKData, USkeletalMeshComponent* Mesh, float DeltaTime, float InterpSpeed, bool bComplexTrace);

	/* Main FootIK function */
	static void SimpleFootIK(FSimpleFootIKData& FootIKData, bool IsInAir, USkeletalMeshComponent* Mesh, float DeltaTime, float InterpSpeed, bool bComplexTrace);

public:
	/* initialize const dependencies for FootIK */
	static void InitializeFootIK(FSimpleFootIKData& FootIK_Data, UAnimInstance* AnimInstance);

	/* Called every frame to update FootIK */
	UFUNCTION(BlueprintCallable, Category = "Simple Foot IK Function Library", meta = (DisplayName = "Update Foot IK Data"))
	static void UpdateFootIKData(UAnimInstance* AnimInstance, UPARAM(ref) FSimpleFootIKData& FootIK_Data, UPARAM(ref) FSimpleFootIKOffsets& Offsets, float MaxCharacterSpeedToUpdate, float InterpSpeed, bool bComplexTrace);
};
