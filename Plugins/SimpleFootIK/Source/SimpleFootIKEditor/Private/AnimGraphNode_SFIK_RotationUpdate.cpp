// Copyright 2023 Vsevolod Khlebnikov. All Rights Reserved.

#include "AnimGraphNode_SFIK_RotationUpdate.h"
#include "SimpleFootIKEditor.h"
#include "Components/SkeletalMeshComponent.h"

#define LOCTEXT_NAMESPACE "A3Nodes"

FText UAnimGraphNode_SFIK_RotationUpdate::GetControllerDescription() const
{
	return LOCTEXT("Update SFIK Rotation", "Update SFIK Rotation");
}

FText UAnimGraphNode_SFIK_RotationUpdate::GetNodeTitle(ENodeTitleType::Type titleType) const
{
	return GetControllerDescription();
}

FText UAnimGraphNode_SFIK_RotationUpdate::GetTooltipText() const
{
	return LOCTEXT("AnimGraphNode_SFIK_Tooltip", "Simple node to update rotation of SFIK bones");
}

void UAnimGraphNode_SFIK_RotationUpdate::Draw(FPrimitiveDrawInterface* PDI, USkeletalMeshComponent* previewSkelMeshComp) const
{
}

#undef LOCTEXT_NAMESPACE
