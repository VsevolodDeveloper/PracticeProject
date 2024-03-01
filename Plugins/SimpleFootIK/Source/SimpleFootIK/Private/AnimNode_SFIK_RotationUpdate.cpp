// Copyright 2023 Vsevolod Khlebnikov. All Rights Reserved.

#include "AnimNode_SFIK_RotationUpdate.h"
#include "Animation/AnimInstanceProxy.h"
#include "AnimationCoreLibrary.h"
#include "AnimationRuntime.h"

DECLARE_CYCLE_STAT(TEXT("SFIK_RotationUpdate"), STAT_SFIK_RotationUpdate, STATGROUP_Anim);

FAnimNode_SFIK_RotationUpdate::FAnimNode_SFIK_RotationUpdate()
{
}

void FAnimNode_SFIK_RotationUpdate::InitializeBoneReferences(const FBoneContainer& RequiredBones)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(InitializeBoneReferences)

	RightFoot = SFIK_Data.RightFoot;
	LeftFoot = SFIK_Data.LeftFoot;

	RightFoot.Initialize(RequiredBones);
	LeftFoot.Initialize(RequiredBones);
}

void FAnimNode_SFIK_RotationUpdate::EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(EvaluateSkeletalControl_AnyThread)
	SCOPE_CYCLE_COUNTER(STAT_SFIK_RotationUpdate);

	TransformBone(Output, OutBoneTransforms, LeftFoot, ETransformMode::Rotate, SFIK_Data.LeftFootRotation);
	TransformBone(Output, OutBoneTransforms, RightFoot, ETransformMode::Rotate, SFIK_Data.RightFootRotation);

	OutBoneTransforms.Sort(FCompareBoneTransformIndex());
}
