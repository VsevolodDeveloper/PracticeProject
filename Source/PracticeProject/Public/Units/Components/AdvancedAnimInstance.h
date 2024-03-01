// Copyright 2023 Vsevolod Khlebnikov. All Rights Reserved.

#pragma once

#include "Animation/AnimInstance.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "CoreMinimal.h"

#include "AdvancedAnimInstance.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMontageCallbackSignature);

/**
 *   Class that have additional logic for binding to montage delegates 
 *   (as you can with K2_Node PlayMontage in blueprints)
 */
UCLASS()
class PRACTICEPROJECT_API UAdvancedAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

private:
	/** Owner of AnimInstance */
	UObject* Owner;

public:
	/** Delegate that call when current montage blend out or interrupted */
	FOnMontageEnded OnMontageBlendOutDelegate;

	/** Delegate that call when current montage completed */
	FOnMontageEnded OnMontageCompletedDelegate;

	/** Method to execute on montage notify begin */
	FMontageCallbackSignature OnMontageNotifyBeginCallback;

	/** Method to execute on montage notify end */
	FMontageCallbackSignature OnMontageNotifyEndCallback;

	/** Method to execute on montage blend out */
	FMontageCallbackSignature OnMontageBlendOutCallback;

	/** Method to execute on montage interrupted */
	FMontageCallbackSignature OnMontageInterruptedCallback;

	/** Method to execute on montage completed */
	FMontageCallbackSignature OnMontageCompletedCallback;

	/** */
	UPROPERTY(Transient)
	UAnimMontage* CurrentMontage;

	/** Get owner of AnimInstance */
	UFUNCTION(BlueprintPure, Category = "Advanced Anim Instance")
	UObject* GetOwner() const { return Owner; }

	/** Set owner of AnimInstance */
	UFUNCTION(BlueprintCallable, Category = "Advanced Anim Instance")
	void SetOwner(UObject* AnimInstanceOwner) { Owner = AnimInstanceOwner; }

	/** Function that execute OnMontageBlendOutMethodToExecute */
	UFUNCTION()
	virtual void FunctionToExecuteOnAnimationBlendOut(UAnimMontage* AnimMontage, bool bInterrupted);

	/** Function that execute OnMontageCompletedMethodToExecute */
	UFUNCTION()
	virtual void FunctionToExecuteOnAnimationCompleted(UAnimMontage* AnimMontage, bool bInterrupted);

	/** Function that execute OnMontageNotifyBeginMethodToExecute */
	UFUNCTION()
	virtual void FunctionToExecuteOnNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);

	/** Function that execute OnMontageNotifyEndMethodToExecute */
	UFUNCTION()
	virtual void FunctionToExecuteOnNotifyEnd(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);

	/** Bind function to delegate that call on montage notify begin */
	void BindOnCurrentMontageNotifyBegin();

	/** Bind function to delegate that call on montage notify end */
	void BindOnCurrentMontageNotifyEnd();

	/** Bind function to delegate that call on montage blend out or interrupted */
	void BindOnMontageBlendOutOrInterrupted(UAnimMontage* Montage);

	/** Bind function to delegate that call on montage completed */
	void BindOnMontageCompleted(UAnimMontage* Montage);

	/** Clear all montage callbacks */
	void ClearAllMontageCallbacks();

	/** Play selected montage */
	virtual void PlayAnimationMontage(UAnimMontage* MontageToPlay);
};
