// Copyright 2023 Vsevolod Khlebnikov. All Rights Reserved.

#include "AnimGraphNode_SFIK_LocationUpdate.h"
#include "SimpleFootIKEditor.h"
#include "Components/SkeletalMeshComponent.h"

#define LOCTEXT_NAMESPACE "A3Nodes"

FText UAnimGraphNode_SFIK_LocationUpdate::GetControllerDescription() const
{
	return LOCTEXT("Update SFIK Location", "Update SFIK Location");
}

FText UAnimGraphNode_SFIK_LocationUpdate::GetNodeTitle(ENodeTitleType::Type titleType) const
{
	return GetControllerDescription();
}

FText UAnimGraphNode_SFIK_LocationUpdate::GetTooltipText() const
{
	return LOCTEXT("AnimGraphNode_SFIK_Tooltip", "Simple node to update location of SFIK root");
}

void UAnimGraphNode_SFIK_LocationUpdate::Draw(FPrimitiveDrawInterface* PDI, USkeletalMeshComponent* previewSkelMeshComp) const
{
}

#undef LOCTEXT_NAMESPACE
