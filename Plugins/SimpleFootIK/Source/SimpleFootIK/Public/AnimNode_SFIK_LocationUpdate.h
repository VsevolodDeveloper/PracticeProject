// Copyright 2023 Vsevolod Khlebnikov. All Rights Reserved.

#pragma once

#include "AnimNode_SFIK_TransformsUpdate.h"

#include "AnimNode_SFIK_LocationUpdate.generated.h"

/**
 * Simple node for updating location of bones for SFIK
 */
USTRUCT(BlueprintInternalUseOnly)
struct SIMPLEFOOTIK_API FAnimNode_SFIK_LocationUpdate : public FAnimNode_SFIK_TransformsUpdate
{
	GENERATED_BODY();

private:
	/* Root bone */
	FBoneReference Root;

public:
	// Sets default values for this actor's properties
	FAnimNode_SFIK_LocationUpdate();

	// FAnimNode_SkeletalControlBase interface
	virtual void InitializeBoneReferences(const FBoneContainer& RequiredBones) override;
	virtual void EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms) override;
	// End of FAnimNode_SkeletalControlBase interface
};