// Copyright 2023 Vsevolod Khlebnikov. All Rights Reserved.

#include "AnimNode_SFIK_LocationUpdate.h"
#include "Animation/AnimInstanceProxy.h"
#include "AnimationCoreLibrary.h"
#include "AnimationRuntime.h"

DECLARE_CYCLE_STAT(TEXT("SFIK_LocationUpdate"), STAT_SFIK_LocationUpdate, STATGROUP_Anim);

FAnimNode_SFIK_LocationUpdate::FAnimNode_SFIK_LocationUpdate()
{
}

void FAnimNode_SFIK_LocationUpdate::InitializeBoneReferences(const FBoneContainer& RequiredBones)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(InitializeBoneReferences)

	Root = SFIK_Data.Root;
	Root.Initialize(RequiredBones);
}

void FAnimNode_SFIK_LocationUpdate::EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(EvaluateSkeletalControl_AnyThread)
	SCOPE_CYCLE_COUNTER(STAT_SFIK_LocationUpdate);

	TransformBone(Output, OutBoneTransforms, Root, ETransformMode::Translate, FRotator(), FVector(0.0f, 0.0f, SFIK_Data.RootDisplacementOffset));

	OutBoneTransforms.Sort(FCompareBoneTransformIndex());
}
