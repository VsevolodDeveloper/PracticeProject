// Copyright 2023 Vsevolod Khlebnikov. All Rights Reserved.

#pragma once

#include "Camera/CameraComponent.h"
#include "Camera/PlayerCameraManager.h"
#include "Components/SceneComponent.h"
#include "Components/WidgetComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/NavMovementComponent.h"
#include "Interfaces/CharacterInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UObject/Object.h"
#include "Units/Components/AdvancedAnimInstance.h"
#include "Units/Components/HitAnimationDirectorComponent.h"
#include "Units/Components/UnitHealthComponent.h"
#include "Units/Enums/EUnitState.h"
#include "Units/Structs/FAnimationsByDamage.h"
#include "Units/Structs/FAnimationsByDirection.h"

#include "EnemyBase.generated.h"

DECLARE_DYNAMIC_DELEGATE(FStartEnemyRagdollSignature);

/**
 * Base class for default enemy unit
 */
UCLASS()
class PRACTICEPROJECT_API AEnemyBase : public ACharacter, public ICharacterInterface
{
	GENERATED_BODY()

private:
	/** Component that deal logic of how enemies react to hits */
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UHitAnimationDirectorComponent* HitAnimationDirectorComponent;

	/** Component that delegate logic of enemy damage taking and health storing */
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UUnitHealthComponent* HealthComponent;

	/** Behavior tree of the enemy */
	UPROPERTY(EditDefaultsOnly, Category = "Behavior")
	class UBehaviorTree* BT;

	/** Pointer to smart object for AI interaction */
	UPROPERTY(EditInstanceOnly, Category = "Behavior")
	class ASmartObject* SmartObject;

	/** Blood drop effect after hit */
	UPROPERTY(EditDefaultsOnly, Category = "FX")
	UParticleSystem* BloodParticles;

	/** Sound to play when a unit dies */
	UPROPERTY(EditDefaultsOnly, Category = "FX")
	USoundBase* DeathSound;

	/** List of death animations by direction */
	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	FAnimationsByDirection DeathAnimationsByDirection;

	/** Type of the unit */
	UPROPERTY(EditDefaultsOnly, Category = "Behavior")
	EUnitType UnitType;

	/** Delegate to call ragdoll animation on death */
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = true), Category = "Behavior")
	FStartEnemyRagdollSignature StartRagdollAnimationCallback;

	/** Current playing montage animation */
	UPROPERTY(Transient)
	UAnimMontage* CurrentUnitMontage;

	/** Weapon that enemy use right now */
	UPROPERTY(Transient)
	AWeaponBase* CurrentWeapon;

	/** Base anim instance of enemy */
	UPROPERTY()
	UAdvancedAnimInstance* AnimInstance;

	/** Current unit state */
	EUnitState UnitState;

	/** Actual cached combo attack */
	TArray<FString> CurrentCombo;

	/** Actual cached list of the combo attacks */
	TMap<FString, UAnimMontage*> CurrentComboAttackList;

	/** Is enemy start to die */
	bool bDead;

	/** Is enemy invulnerable to damage */
	bool bInvincibility;

	/** Is enemy see player */
	bool bCanSeePlayer;

	/** Is enemy has any attacks in combo */
	bool bHasComboAttacks;

	/** How much enemy take damage by last hit */
	float LastTakenDamage;

	/** Death sound volume value */
	UPROPERTY(EditAnywhere, Category = "FX")
	float DeathSoundVolume;

	/** Health value threshold for applying death */
	UPROPERTY(EditDefaultsOnly, Category = "Behavior")
	float HealthTresholdForDeath;

	/** Mass scale to set when the enemy enter ragdoll */
	UPROPERTY(EditAnywhere, Category = "Behavior")
	float EnemyMassScaleOnDeath;

	/** Delay before the ragdoll is turned on when the enemy start to die */
	UPROPERTY(EditAnywhere, Category = "Behavior")
	float RagdollInitialDelay;

	/** Time until the enemy disappear completely after fatal hit */
	UPROPERTY(EditAnywhere, Category = "Behavior")
	float DyingTime;

	/** Finish combo attack chain and clear all cache */
	void FinishComboAttack();

	/** Set current montage pointer to null */
	void InvalidateCurrentMontage() { CurrentUnitMontage = nullptr; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Set health bar widget visible */
	UFUNCTION(BlueprintCallable, Category = "Enemy|FX")
	void SetHealthBarVisibility(AActor* TargetActor, float SightRadius, UWidgetComponent* HealthBar);

	/** Set enemy widgets location and rotation from look of the target character */
	UFUNCTION(BlueprintCallable, Category = "Enemy|FX")
	void SetEnemyWidgetsPosition(APawn* TargetCharacter, UWidgetComponent* HealthBar, UWidgetComponent* DamageBar);

	/** Called when enemy start to die */
	UFUNCTION(BlueprintCallable, Category = "Enemy|Behavior")
	bool Death(AActor* DeathCauser, UObject* PhysicalAnimationComponent);

	/** Called when enemy take any damage */
	UFUNCTION(BlueprintCallable, Category = "Enemy|Behavior")
	void OnTakeDamage(float Damage, AActor* DamageCauser, UObject* PhysicalAnimationComponent);

	/** Called when enemy saw player */
	UFUNCTION(BlueprintCallable, Category = "Enemy|Behavior")
	bool OnSeePlayer(APawn* PlayerCharacter);

	/** Heavy enemy attack */
	UFUNCTION(BlueprintCallable, Category = "Enemy|Behavior")
	void HeavyAttack();

	/** Enemy melee attack */
	UFUNCTION(BlueprintCallable, Category = "Enemy|Behavior")
	void ComboAttack();

	/** Choose random attack from configuration and attack */
	UFUNCTION(BlueprintCallable, meta = (Category = "Enemy|Behavior"))
	void RandomComboAttack();

	/** Called when hit animation start to blend out */
	UFUNCTION(BlueprintNativeEvent, Category = "Animations")
	void OnTakeDamageBlendOut();

	/** Called when hit animation interrupted */
	UFUNCTION(BlueprintNativeEvent, Category = "Animations")
	void OnTakeDamageInterrupted();

	/** Called when hit completed */
	UFUNCTION(BlueprintNativeEvent, Category = "Animations")
	void OnTakeDamageCompleted();

	/** Called on the begin notify of the heavy attack */
	UFUNCTION(BlueprintNativeEvent, Category = "Animations")
	void HeavyAttackOnBeginNotify();

	/** Called on the end of notify of the heavy attack */
	UFUNCTION(BlueprintNativeEvent, Category = "Animations")
	void HeavyAttackOnEndNotify();

	/** Called when heavy attack interrupted */
	UFUNCTION(BlueprintNativeEvent, Category = "Animations")
	void HeavyAttackOnInterrupted();

	/** Called when heavy attack completed */
	UFUNCTION(BlueprintNativeEvent, Category = "Animations")
	void HeavyAttackOnCompleted();

	/** Called on the begin notify of the combo attack */
	UFUNCTION(BlueprintNativeEvent, Category = "Animations")
	void ComboAttackOnBeginNotify();

	/** Called on the end of notify of the combo attack */
	UFUNCTION(BlueprintNativeEvent, Category = "Animations")
	void ComboAttackOnEndNotify();

	/** Called when combo attack interrupted */
	UFUNCTION(BlueprintNativeEvent, Category = "Animations")
	void ComboAttackOnInterrupted();

	/** Called when combo attack completed */
	UFUNCTION(BlueprintNativeEvent, Category = "Animations")
	void ComboAttackOnCompleted();

public:
	// Sets default values for this character's properties
	AEnemyBase();

	/** Called on enemy's dead */
	UFUNCTION(BlueprintImplementableEvent, Category = "Enemy|Behavior")
	void OnDeath();

	/** Get is enemy has any attacks in combo */
	UFUNCTION(BlueprintPure, Category = "Enemy|Behavior")
	bool HasComboAttacks() const { return bHasComboAttacks; }

	/** Get is enemy see player */
	UFUNCTION(BlueprintPure, Category = "Enemy|Behavior")
	bool GetCanSeePlayer() const { return bCanSeePlayer; }

	/** Get how much enemy take damage by last hit */
	UFUNCTION(BlueprintPure, Category = "Enemy|Behavior")
	float GetLastTakenDamage() const { return LastTakenDamage; }

	/** Get component that deal logic of how enemies react to hits */
	UFUNCTION(BlueprintPure, Category = "Enemy|Behavior")
	UHitAnimationDirectorComponent* GetHitAnimationDirectorComponent() const { return HitAnimationDirectorComponent; }

	/** Get component that delegate logic of enemy damage taking and health storing */
	UFUNCTION(BlueprintPure, Category = "Enemy|Behavior")
	UUnitHealthComponent* GetUnitHealthComponent() const { return HealthComponent; }

	/** Set component that delegate logic of enemy damage taking and health storing */
	UFUNCTION(BlueprintCallable, Category = "Enemy|Behavior")
	void SetUnitHealthComponent(UUnitHealthComponent* InHealthComponent) { HealthComponent = InHealthComponent; }

	/** Get behavior tree of the enemy */
	UFUNCTION(BlueprintPure, Category = "Enemy|Behavior")
	class UBehaviorTree* GetBehaviorTree() const { return BT; }

	/** Get pointer to smart object for AI interaction */
	UFUNCTION(BlueprintPure, Category = "Enemy|Behavior")
	class ASmartObject* GetSmartObject() const { return SmartObject; }

	/** Get current playing montage animation */
	UFUNCTION(BlueprintPure, Category = "Enemy|Animations")
	UAnimMontage* GetCurrentUnitMontage() const { return CurrentUnitMontage; }

	/** Get base anim instance of enemy */
	UFUNCTION(BlueprintPure, Category = "Enemy|Animations")
	UAdvancedAnimInstance* GetAnimInstance() const { return AnimInstance; }

	// Character Interface Implementation
	ACharacter* GetCharacterSelf_Implementation() override;
	USkeletalMeshComponent* GetCharacterMesh_Implementation() const override;
	AWeaponBase* GetCurrentWeapon_Implementation() const override;
	EUnitState GetUnitState_Implementation() const override;
	EUnitType GetUnitType_Implementation() const override;
	bool GetIsDead_Implementation() const override;
	bool GetInvincibility_Implementation() const override;
	void SetUnitState_Implementation(EUnitState NewUnitState) override;
	void SetInvincibility_Implementation(bool bSetInvisible) override;
	virtual void SetCurrentWeapon_Implementation(AWeaponBase* Weapon) override;
	// End of Character Interface Implementation
};
