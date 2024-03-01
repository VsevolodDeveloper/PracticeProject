// Copyright 2023 Vsevolod Khlebnikov. All Rights Reserved.

#include "AnimNode_SFIK_TransformsUpdate.h"
#include "Animation/AnimInstanceProxy.h"
#include "AnimationCoreLibrary.h"
#include "AnimationRuntime.h"

DECLARE_CYCLE_STAT(TEXT("SFIK_TransformsUpdate"), STAT_SFIK_TransformsUpdate, STATGROUP_Anim);

FAnimNode_SFIK_TransformsUpdate::FAnimNode_SFIK_TransformsUpdate()
{
}

void FAnimNode_SFIK_TransformsUpdate::GatherDebugData(FNodeDebugData& DebugData)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(GatherDebugData)

	FString DebugLine = DebugData.GetNodeName(this);

	DebugLine += "(";
	AddDebugNodeData(DebugLine);
	DebugLine += FString::Printf(TEXT(")"));
	DebugData.AddDebugItem(DebugLine);

	ComponentPose.GatherDebugData(DebugData);
}

void FAnimNode_SFIK_TransformsUpdate::TransformBone(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms, FBoneReference BoneToModify, ETransformMode TransformMode, FRotator Rotation, FVector Translation)
{
	if (!BoneToModify.IsValidToEvaluate())
	{
		return;
	}

	const FBoneContainer& BoneContainer = Output.Pose.GetPose().GetBoneContainer();

	FCompactPoseBoneIndex CompactPoseBoneToModify = BoneToModify.GetCompactPoseIndex(BoneContainer);
	FTransform NewBoneTM = Output.Pose.GetComponentSpaceTransform(CompactPoseBoneToModify);
	FTransform ComponentTransform = Output.AnimInstanceProxy->GetComponentTransform();

	if (TransformMode == ETransformMode::Rotate)
	{
		// Convert to Bone Space.
		FAnimationRuntime::ConvertCSTransformToBoneSpace(ComponentTransform, Output.Pose, NewBoneTM, CompactPoseBoneToModify, EBoneControlSpace::BCS_WorldSpace);

		const FQuat BoneQuat(Rotation);
		NewBoneTM.SetRotation(BoneQuat * NewBoneTM.GetRotation());

		// Convert back to Component Space.
		FAnimationRuntime::ConvertBoneSpaceTransformToCS(ComponentTransform, Output.Pose, NewBoneTM, CompactPoseBoneToModify, EBoneControlSpace::BCS_WorldSpace);
	}

	if (TransformMode == ETransformMode::Translate)
	{
		// Convert to Bone Space.
		FAnimationRuntime::ConvertCSTransformToBoneSpace(ComponentTransform, Output.Pose, NewBoneTM, CompactPoseBoneToModify, EBoneControlSpace::BCS_WorldSpace);

		NewBoneTM.AddToTranslation(Translation);

		// Convert back to Component Space.
		FAnimationRuntime::ConvertBoneSpaceTransformToCS(ComponentTransform, Output.Pose, NewBoneTM, CompactPoseBoneToModify, EBoneControlSpace::BCS_WorldSpace);
	}

	// Return it back
	OutBoneTransforms.Add(FBoneTransform(BoneToModify.GetCompactPoseIndex(BoneContainer), NewBoneTM));
}

bool FAnimNode_SFIK_TransformsUpdate::IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones)
{
	if (SFIK_Data.bIsInitialized)
	{
		return true;
	}
	else
	{
		return false;
	}
}