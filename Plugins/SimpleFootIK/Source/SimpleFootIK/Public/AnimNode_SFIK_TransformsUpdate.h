// Copyright 2023 Vsevolod Khlebnikov. All Rights Reserved.

#pragma once

#include "BoneContainer.h"
#include "BoneControllers/AnimNode_SkeletalControlBase.h"
#include "BonePose.h"
#include "SimpleFootIK_FunctionLibrary.h"

#include "AnimNode_SFIK_TransformsUpdate.generated.h"

/** Enumerates states of a transform modes */
UENUM(BlueprintType)
enum class ETransformMode : uint8
{
	Translate,
	Rotate,
};

/**
 * Simple node for updating transforms of bones for SFIK
 */
USTRUCT(BlueprintInternalUseOnly)
struct SIMPLEFOOTIK_API FAnimNode_SFIK_TransformsUpdate : public FAnimNode_SkeletalControlBase
{
	GENERATED_BODY();

protected:
	/* Main data to update SFIK transforms */
	UPROPERTY(EditAnyWhere, Category = "Simple Foot IK", meta = (PinShownByDefault))
	FSimpleFootIKData SFIK_Data;

public:
	// Sets default values for this actor's properties
	FAnimNode_SFIK_TransformsUpdate();

	// FAnimNode_Base interface
	virtual void GatherDebugData(FNodeDebugData& DebugData) override;
	virtual bool NeedsOnInitializeAnimInstance() const override { return true; }
	// End of FAnimNode_Base interface

	// FAnimNode_SkeletalControlBase interface
	virtual bool IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones) override;
	// End of FAnimNode_SkeletalControlBase interface

	/* */
	static void TransformBone(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms, FBoneReference BoneToModify, ETransformMode TransformMode, FRotator Rotation = FRotator(), FVector Translation = FVector());
};