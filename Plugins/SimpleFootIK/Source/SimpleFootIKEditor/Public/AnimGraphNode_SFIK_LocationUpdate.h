// Copyright 2023 Vsevolod Khlebnikov. All Rights Reserved.

#pragma once

#include "AnimGraph/Classes/AnimGraphNode_SkeletalControlBase.h"
#include "AnimNode_SFIK_LocationUpdate.h"
#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"

#include "AnimGraphNode_SFIK_LocationUpdate.generated.h"

/**
 * Simple graph node to update SFIK location of your humanoid root
 */
UCLASS(MinimalAPI)
class UAnimGraphNode_SFIK_LocationUpdate : public UAnimGraphNode_SkeletalControlBase
{
	GENERATED_BODY()

private:
	/* Simple node for updating location of root for SFIK */
	UPROPERTY(EditAnywhere, Category = Settings)
	FAnimNode_SFIK_LocationUpdate Node;

protected:
	// UAnimGraphNode_SkeletalControlBase interface
	virtual FText GetControllerDescription() const override;
	virtual const FAnimNode_SkeletalControlBase* GetNode() const override { return &Node; }
	virtual void Draw(FPrimitiveDrawInterface* PDI, USkeletalMeshComponent* previewSkelMeshComp) const override;
	// End of UAnimGraphNode_SkeletalControlBase interface

public:
	// UEdGraphNode interface
	virtual FText GetNodeTitle(ENodeTitleType::Type titleType) const override;
	virtual FText GetTooltipText() const override;
	// End of UEdGraphNode interface
};