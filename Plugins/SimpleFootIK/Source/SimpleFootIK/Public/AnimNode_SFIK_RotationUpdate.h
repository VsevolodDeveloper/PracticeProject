// Copyright 2023 Vsevolod Khlebnikov. All Rights Reserved.

#pragma once

#include "AnimNode_SFIK_TransformsUpdate.h"

#include "AnimNode_SFIK_RotationUpdate.generated.h"

/**
 * Simple node for updating rotation of bones for SFIK
 */
USTRUCT(BlueprintInternalUseOnly)
struct SIMPLEFOOTIK_API FAnimNode_SFIK_RotationUpdate : public FAnimNode_SFIK_TransformsUpdate
{
	GENERATED_BODY();

private:
	/* Right foot bone */
	FBoneReference RightFoot;

	/* Left foot bone */
	FBoneReference LeftFoot;

public:
	// Sets default values for this actor's properties
	FAnimNode_SFIK_RotationUpdate();

	// FAnimNode_SkeletalControlBase interface
	virtual void InitializeBoneReferences(const FBoneContainer& RequiredBones) override;
	virtual void EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms) override;
	// End of FAnimNode_SkeletalControlBase interface
};