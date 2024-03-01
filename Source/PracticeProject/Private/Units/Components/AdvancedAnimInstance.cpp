// Copyright 2023 Vsevolod Khlebnikov. All Rights Reserved.

#include "Units/Components/AdvancedAnimInstance.h"
#include "PracticeProject/PracticeProjectCustomLogs.h"

DEFINE_LOG_CATEGORY(AdvancedAnimInstance_LOG);

void UAdvancedAnimInstance::BindOnCurrentMontageNotifyBegin()
{
	OnPlayMontageNotifyBegin.Clear();
	OnPlayMontageNotifyBegin.AddUniqueDynamic(this, &UAdvancedAnimInstance::FunctionToExecuteOnNotifyBegin);
}

void UAdvancedAnimInstance::BindOnCurrentMontageNotifyEnd()
{
	OnPlayMontageNotifyEnd.Clear();
	OnPlayMontageNotifyEnd.AddUniqueDynamic(this, &UAdvancedAnimInstance::FunctionToExecuteOnNotifyEnd);
}

void UAdvancedAnimInstance::BindOnMontageBlendOutOrInterrupted(UAnimMontage* Montage)
{
	OnMontageBlendOutDelegate.Unbind();
	OnMontageBlendOutDelegate.BindUObject(this, &UAdvancedAnimInstance::FunctionToExecuteOnAnimationBlendOut);
	UAnimInstance::Montage_SetBlendingOutDelegate(OnMontageBlendOutDelegate, Montage);
}

void UAdvancedAnimInstance::BindOnMontageCompleted(UAnimMontage* Montage)
{
	OnMontageCompletedDelegate.Unbind();
	OnMontageCompletedDelegate.BindUObject(this, &UAdvancedAnimInstance::FunctionToExecuteOnAnimationCompleted);
	UAnimInstance::Montage_SetEndDelegate(OnMontageCompletedDelegate, Montage);
}

void UAdvancedAnimInstance::FunctionToExecuteOnNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
	if (OnMontageNotifyBeginCallback.IsBound())
	{
		OnMontageNotifyBeginCallback.Broadcast();
	}
}

void UAdvancedAnimInstance::FunctionToExecuteOnNotifyEnd(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
	if (OnMontageNotifyEndCallback.IsBound())
	{
		OnMontageNotifyEndCallback.Broadcast();
	}
}

void UAdvancedAnimInstance::FunctionToExecuteOnAnimationBlendOut(UAnimMontage* AnimMontage, bool bInterrupted)
{
	if (bInterrupted)
	{
		if (OnMontageInterruptedCallback.IsBound() && AnimMontage == CurrentMontage)
		{
			OnMontageInterruptedCallback.Broadcast();
		}
	}
	else
	{
		if (OnMontageBlendOutCallback.IsBound() && AnimMontage == CurrentMontage)
		{
			OnMontageBlendOutCallback.Broadcast();
		}
	}
}

void UAdvancedAnimInstance::FunctionToExecuteOnAnimationCompleted(UAnimMontage* AnimMontage, bool bInterrupted)
{
	if (!bInterrupted)
	{
		if (OnMontageCompletedCallback.IsBound() && AnimMontage == CurrentMontage)
		{
			OnMontageCompletedCallback.Broadcast();
		}
	}
}

void UAdvancedAnimInstance::PlayAnimationMontage(UAnimMontage* MontageToPlay)
{
	if (MontageToPlay)
	{
		CurrentMontage = MontageToPlay;

		Montage_Play(MontageToPlay, 1.0f);

		BindOnCurrentMontageNotifyBegin();
		BindOnCurrentMontageNotifyEnd();

		BindOnMontageBlendOutOrInterrupted(MontageToPlay);
		BindOnMontageCompleted(MontageToPlay);
	}
}

void UAdvancedAnimInstance::ClearAllMontageCallbacks()
{
	OnMontageNotifyBeginCallback.Clear();
	OnMontageNotifyEndCallback.Clear();
	OnMontageInterruptedCallback.Clear();
	OnMontageBlendOutCallback.Clear();
	OnMontageCompletedCallback.Clear();
}
