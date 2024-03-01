// Copyright 2023 Vsevolod Khlebnikov. All Rights Reserved.

#pragma once

#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "CollisionShape.h"
#include "Components/ActorComponent.h"
#include "CoreMinimal.h"
#include "DrawDebugHelpers.h"
#include "Engine/EngineTypes.h"
#include "Engine/World.h"
#include "Interfaces/CharacterInterface.h"
#include "Interfaces/WeaponInterface.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Weapons/WeaponBase.h"

#include "DealDamageNotify.generated.h"

/**
 *  Anim Notify that deal damage to unit 
 */
UCLASS()
class PRACTICEPROJECT_API UDealDamageNotify : public UAnimNotifyState
{
	GENERATED_BODY()

private:
	/** Half size of tracer for damage hit */
	UPROPERTY(EditAnywhere, Category = "Trace")
	FVector TraceHalfSize;

	/** Life time of tracer for damage hit */
	UPROPERTY(EditAnywhere, Category = "Trace")
	float TraceTime;

	/** Return weapon of notify owner */
	AWeaponBase* GetOwnerWeapon(class USkeletalMeshComponent* MeshComponent) const;

	/** Return weapon mesh of the given weapon */
	UStaticMeshComponent* GetWeaponMesh(AWeaponBase* GivenWeapon) const;

public:
	// Sets default values for this actor's properties
	UDealDamageNotify();

	/** NotifyBegin implementation */
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;

	/** NotifyEnd implementation */
	virtual void NotifyEnd(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation) override;

	/** NotifyTick implementation */
	virtual void NotifyTick(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, float FrameDeltaTime) override;

	/** Trace to find units that can be hit */
	UFUNCTION(BlueprintCallable, Category = "Deal Damage Notify")
	bool TraceForDamageHit(USkeletalMeshComponent* CharacterMesh, UStaticMeshComponent* WeaponMesh, FHitResult& Hit);
};
