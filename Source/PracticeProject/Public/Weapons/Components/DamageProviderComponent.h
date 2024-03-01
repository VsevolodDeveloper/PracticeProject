// Copyright 2023 Vsevolod Khlebnikov. All Rights Reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "Components/PrimitiveComponent.h"
#include "CoreMinimal.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Misc/OutputDeviceNull.h"

#include "DamageProviderComponent.generated.h"

/**
 *  Component that provide and deal damage to target actor 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PRACTICEPROJECT_API UDamageProviderComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	/** Whether can hit actor and deal damage to it, or not */
	bool bCanHit;

	/** Physical strength of hit */
	UPROPERTY(EditAnywhere, Category = "Behavior")
	float HitImpulseStrength;

	/** Minimum of accumulated damage multiplier rate on recovery hit */
	UPROPERTY(EditAnywhere, Category = "Behavior")
	float MinDamageMultiplierOnRecoveryHit;

	/** Maximum of accumulated damage multiplier rate on recovery hit */
	UPROPERTY(EditAnywhere, Category = "Behavior")
	float MaxDamageMultiplierOnRecoveryHit;

	/** Maximum of accumulated damage multiplier rate on character hit */
	UPROPERTY(EditAnywhere, Category = "Behavior")
	float MinDamageMultiplierOnCharacterHit;

	/** Maximum of accumulated damage multiplier rate on character hit */
	UPROPERTY(EditAnywhere, Category = "Behavior")
	float MaxDamageMultiplierOnCharacterHit;

	/** Name of method that enable physical recovery after hit */
	UPROPERTY(EditAnywhere, Category = "Behavior")
	FString EnablePhysicalRecoveryMethodName;

	/** Attenuation settings of recovery sound */
	UPROPERTY(EditAnywhere, Category = "FX")
	class USoundAttenuation* AttenuationSettings;

	/** Sound of hit to recovery component (such as shield etc.) */
	UPROPERTY(EditAnywhere, Category = "FX")
	USoundBase* RecoveryHitSound;

	/** Emitter of blood particles */
	UPROPERTY(EditAnywhere, Category = "FX")
	UParticleSystem* BloodEmitter;

	/** Emitter of sparks from hit of recovery component */
	UPROPERTY(EditAnywhere, Category = "FX")
	UParticleSystem* RecoverysSparklesEmitter;

	/** Class of recovery component that can be hitted */
	UPROPERTY(EditAnywhere, Category = "Behavior")
	UClass* RecoveryComponentClass;

	/** Apply hit and damage */
	void ApplyHit(AActor* DamageCauser, const FHitResult& Hit, float AccumulatedDamage);

	/** Applying hit to character's (or maybe not, in some cases) components if possible */
	void TryToHitComponent(const FHitResult& Hit, AActor* DamageCauser, float AccumulatedDamage);

	/** Applying hit to character or his components if possible */
	void TryToHitCharacterOrHisComponents(AActor* DamageCauser, const FHitResult& Hit, float AccumulatedDamage);

	/** Apply hit and damage to character */
	void ApplyHitToCharacter(const FHitResult& Hit, float AccumulatedDamage, AActor* DamageCauser);

	/** Apply hit and damage to other component */
	void ApplyHitToOtherComponent(const FHitResult& Hit, float AccumulatedDamage, AActor* DamageCauser);

	/** Apply hit and to recovery component */
	void ApplyHitToRecoveryComponent(AActor* DamageCauser, float AccumulatedDamage, const FHitResult& Hit);

	/** Return is hit target is character or not */
	bool IsHitTargetIsCharacter(const FHitResult& Hit) const;

	/** Return is component can be damaged, or not */
	bool IsComponentCanBeDamaged(const FHitResult& Hit) const;

	/** Return is hit target is recovery component or not */
	bool IsRecoveryComponentWasHitted(const FHitResult& Hit) const;

	/** Return is hit target and damage causer both enemies or not */
	bool IsHitTargetAndDamageCauserBothEnemies(AActor* DamageCauser, const FHitResult& Hit) const;

	/** Return is hit target and damage causer both characters or not */
	bool IsHitTargetAndDamageCauserBothCharacters(AActor* DamageCauser, const FHitResult& Hit) const;

public:
	// Sets default values for this component's properties
	UDamageProviderComponent();

	/** Deal hit and apply damage if it should */
	UFUNCTION(BlueprintCallable, Category = "Hit Provider Component")
	void DealHit(const FHitResult& Hit, bool IsHit, float AttackDamageRate, float WeaponDamageRate, AActor* DamageCauser);

	/** Spawn blood particles after hit */
	UFUNCTION(BlueprintCallable, Category = "Hit Provider Component")
	void SpawnBloodParticles(const FHitResult& Hit);

	/** Spawn particles after hit of recovery component */
	UFUNCTION(BlueprintCallable, Category = "Hit Provider Component")
	void SpawnRecoverysSparkles(AActor* HitCauser, const FHitResult& Hit);

	/** Get whether can hit actor and deal damage to it, or not */
	bool GetCanHitActor() const { return bCanHit; }

	/** Set whether can hit actor and deal damage to it, or not */
	void SetCanHitActor(bool bInCanHit) { bCanHit = bInCanHit; }
};
