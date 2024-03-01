// Copyright 2023 Vsevolod Khlebnikov. All Rights Reserved.

#pragma once

#include "Animation/AnimInstance.h"
#include "Async/Async.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/WidgetTree.h"
#include "Camera/CameraComponent.h"
#include "Camera/PlayerCameraManager.h"
#include "Components/PrimitiveComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/NavMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Interfaces/CharacterInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Misc/OutputDeviceNull.h"
#include "Runtime/Engine/Classes/Camera/CameraModifier_CameraShake.h"
#include "Runtime/Engine/Classes/Components/CapsuleComponent.h"
#include "UMG/DeathScreenWidget.h"
#include "UObject/ConstructorHelpers.h"
#include "Units/Components/UnitHealthComponent.h"
#include "Units/Components/AdvancedAnimInstance.h"
#include "Units/Components/HitAnimationDirectorComponent.h"
#include "Units/Enums/EUnitState.h"
#include "Weapons/WeaponBase.h"

#include "PlayerCharacter.generated.h"

DECLARE_DYNAMIC_DELEGATE(FStrafeByCameraDirectionSignature);
DECLARE_DYNAMIC_DELEGATE(FStrafeByInputDirectionSignature);
DECLARE_DYNAMIC_DELEGATE(FStartPlayerRagdollSignature);

/**
 * Structure that holds data required for initialize player's death 
 */
USTRUCT(BlueprintType)
struct FPlayerDeathData
{
	GENERATED_BODY()

public:
	/** Animation that played on death */
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Death Data")
	UAnimMontage* DeathAnimation;

	/** Sound that played on death*/
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Death Data")
	USoundBase* DeathSound;

	/** Sound mix used by death sound  */
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Death Data")
	USoundMix* DeathThemeMix;

	/** Sound class used by death sound */
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Death Data")
	USoundClass* DeathThemeClass;

	/** Class used for create death screen widget */
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Death Data")
	UClass* DeathScreenClass;
};

/**
 * Base class for player character
 */
UCLASS()
class PRACTICEPROJECT_API APlayerCharacter : public ACharacter, public ICharacterInterface
{
	GENERATED_BODY()

private:
	/** Component that holds camera component */
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	USpringArmComponent* SpringArmComponent;

	/** Component that capture game for player */
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UCameraComponent* CameraComponent;

	/** Component that delegate logic of player damage taking and health storing */
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UUnitHealthComponent* HealthComponent;

	/** Component that deal logic of how player react to hits */
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UHitAnimationDirectorComponent* HitAnimationDirectorComponent;

	/** Delegate to call strafe by camera direction implementation */
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = true), Category = "Behavior")
	FStrafeByCameraDirectionSignature StrafeByCameraDirectionCallback;

	/** Delegate to call strafe by input direction implementation */
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = true), Category = "Behavior")
	FStrafeByInputDirectionSignature StrafeByInputDirectionCallback;

	/** Delegate to call ragdoll animation on death */
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = true), Category = "Behavior")
	FStartPlayerRagdollSignature StartRagdollAnimationCallback;

	/** Class of camera shake effect, used in various actions */
	UPROPERTY(Transient)
	TSubclassOf<UCameraShakeBase> CameraShakeClass;

	/** Current playing hit montage animation */
	UPROPERTY(Transient)
	UAnimMontage* CurrentHitAnimation;

	/** An actor who is the focus target */
	UPROPERTY(Transient)
	AActor* FocusTarget;

	/** Weapon that player use right now */
	UPROPERTY(Transient)
	AWeaponBase* CurrentWeapon;

	/** Base anim instance of player */
	UPROPERTY()
	UAdvancedAnimInstance* AnimInstance;

	/** Timer handle that handle focus event */
	FTimerHandle TimerHandle_Focus;

	/** Current player state */
	EUnitState UnitState;

	/** Type of the unit */
	EUnitType UnitType;

	/** Actual cached combo attack */
	TArray<FString> CurrentCombo;

	/** Actual cached list of the combo attacks */
	TMap<FString, UAnimMontage*> CurrentComboAttackList;

	/** How long player can pre strafe before perform heavy attack */
	UPROPERTY(EditDefaultsOnly, Category = "Behavior")
	float HeavyAttackPreStrafeTime;

	/** Time since roll start, when roll shake FX should be played */
	UPROPERTY(EditDefaultsOnly, Category = "Behavior")
	float RollShakeInitialDelay;

	/** Health value treshold for applying death */
	UPROPERTY(EditDefaultsOnly, Category = "Behavior")
	float HealthTresholdForDeath;

	/** Death sound volume value */
	UPROPERTY(EditDefaultsOnly, Category = "FX")
	float DeathSoundVolume;

	/** Mass scale to set when the player enter ragdoll */
	UPROPERTY(EditDefaultsOnly, Category = "Behavior")
	float PlayerMassScaleOnDeath;

	/** Delay before the ragdoll is turned on when the player start to die */
	UPROPERTY(EditDefaultsOnly, Category = "Behavior")
	float RagdollInitialDelay;

	/** Fade time of all sounds, when player dying */
	UPROPERTY(EditDefaultsOnly, Category = "Behavior")
	float VolumeFadeTimeAfterDeath;

	/** Highest distance to target at which focus wold be setted */
	UPROPERTY(EditDefaultsOnly, Category = "Behavior")
	float HighestDistanceToFocusTarget;

	/** Camera roll offset when player is focusing enemy */
	UPROPERTY(EditDefaultsOnly, Category = "FocusSettings")
	float FocusRollOffset;

	/** How fast camera should focus enemy */
	UPROPERTY(EditDefaultsOnly, Category = "FocusSettings")
	float FocusInterpSpeed;

	/** How fast character can move when he is in focus */
	UPROPERTY(EditDefaultsOnly, Category = "FocusSettings")
	float CharacterSpeedInFocusRate;

	/** Inner radius of camera shake effect */
	UPROPERTY(EditDefaultsOnly, Category = "FX")
	float CameraShakeInnerRadius;

	/** Outer radius of camera shake effect */
	UPROPERTY(EditDefaultsOnly, Category = "FX")
	float CameraShakeOuterRadius;

	/** Fall off of camera shake effect */
	UPROPERTY(EditDefaultsOnly, Category = "FX")
	float CameraShakeFalloff;

	/** Max value of character moving stamina */
	UPROPERTY(EditDefaultsOnly, Category = "Behavior")
	int StaminaCapacity;

	/** Max manna reserve value for magical attacks */
	UPROPERTY(EditDefaultsOnly, Category = "Behavior")
	int MannaCapacity;

	/** Manna cost for magical attacks */
	UPROPERTY(EditAnywhere, Category = "Behavior")
	int MannaCostOfHeavyAttack;

	/** Stamina cost for roll */
	UPROPERTY(EditAnywhere, Category = "Behavior")
	int StaminaCostOfRoll;

	/** Cached value of stamina cost by current combo, for each attack */
	int StaminaCostOfCurrentCombo;

	/** Value of character moving stamina */
	int Stamina;

	/** Manna reserve for magical attacks */
	int Manna;

	/** Is player invulnerable to damage */
	bool bInvincibility;

	/** Whatever player focus someone or not */
	bool bInFocus;

	/** Is player start to die */
	bool bDead;

	/** Is player has any attacks in combo */
	bool bHasComboAttacks;

	/** Is player must continue combo attack chain */
	bool bMustPerformNextAttack;

	/** Is the animation interval when the player can continue combo attack chain has started */
	bool bAttackWindowBegun;

	/** Start to focus closest unit */
	void SetFocus();

	/** Clear player focus on the target */
	void ClearFocus();

	/** Switch focus to the other unit */
	void SwitchFocus();

	/** Find closest target to focus */
	bool FindFocusTarget();

	/** Focus player view on the closest enemy */
	void UpdateFocus();

	/** Move character right */
	void MoveRight(float Value);

	/** Move character forward */
	void MoveForward(float Value);

	/** Return player rotation yaw */
	FRotator GetPlayerRotationYaw() const { return FRotator(0.0f, GetControlRotation().Yaw, 0.0f); }

	/** Reset all data referenced to combo attack and finish it */
	UFUNCTION()
	void StartToFinishComboAttack();

	/** Start playing camera shake effect */
	UFUNCTION()
	void StartCameraShake();

	/** Called on the begin notify of the heavy attack */
	UFUNCTION()
	void HeavyAttackOnNotifyBegin();

	/** Called when heavy attack animation start to blend out */
	UFUNCTION()
	void HeavyAttackOnBlendOut();

	/** Called on the begin notify of the combo attack */
	UFUNCTION()
	void ComboAttackOnBeginNotify();

	/** Called on the end of notify of the combo attack */
	UFUNCTION()
	void ComboAttackOnEndNotify();

	/** Called when combo attack animation start to blend out */
	UFUNCTION()
	void ComboAttackOnBlendOut();

	/** Called on the begin notify of the roll */
	UFUNCTION()
	void RollOnBeginNotify();

	/** Called on the end notify of the roll */
	UFUNCTION()
	void RollOnEndNotify();

	/** Called when roll animation start to blend out */
	UFUNCTION()
	void RollOnBlendOut();

	/** Called when hit animation completed */
	UFUNCTION()
	void OnTakeDamageBlendOut();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Heavy player attack */
	UFUNCTION(BlueprintCallable, Category = "Player|Behavior")
	void HeavyAttack(TSubclassOf<UCameraShakeBase> CameraShakeClassToSet);

	/** Player melee attack */
	UFUNCTION(BlueprintCallable, Category = "Player|Behavior")
	void ComboAttack();

	/** Select random attack from configuration and attack */
	UFUNCTION(BlueprintCallable, Category = "Player|Behavior")
	void RandomComboAttack(TSubclassOf<UCameraShakeBase> CameraShakeClassToSet);

	/** Take stamina amount */
	UFUNCTION(BlueprintCallable, Category = "Player|Movement")
	bool TakeStamina(int Difference);

	/** Increment stamina amount */
	UFUNCTION(BlueprintCallable, Category = "Player|Movement")
	void RegainStamina();

	/** Called when player start to die */
	UFUNCTION(BlueprintCallable, Category = "Player|Behavior")
	void Death(FPlayerDeathData DeathData);

	/** Perform dodge to the right side */
	UFUNCTION(BlueprintCallable, Category = "Player|Movement")
	void RightDodge(UAnimMontage* DodgeAnimation);

	/** Perform dodge to the left side */
	UFUNCTION(BlueprintCallable, Category = "Player|Movement")
	void LeftDodge(UAnimMontage* DodgeAnimation);

	/** Perform roll with direction selected by strafe */
	UFUNCTION(BlueprintCallable, Category = "Player|Movement")
	void Roll(UAnimMontage* RollAnimation, TSubclassOf<UCameraShakeBase> CameraShakeClassToSet);

	/** Called when player get hit */
	UFUNCTION(BlueprintCallable, Category = "Player|Behavior")
	bool OnTakeDamage(float Damage, AActor* DamageCauser, TSubclassOf<UCameraShakeBase> CameraShakeClassToSet);

	/** Get type of recovery depends on unit state */
	UFUNCTION(BlueprintPure, Category = "Player|Behavior")
	FString GetRecoveryType(bool bParryKeyPressed) const;

public:
	// Sets default values for this character's properties
	APlayerCharacter();

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Add Controller Yaw Input
	virtual void AddControllerYawInput(float Val) override;

	// Add Controller Pitch Input
	virtual void AddControllerPitchInput(float Val) override;

	/** Get component that holds camera component */
	UFUNCTION(BlueprintPure, Category = "Player|Components")
	USpringArmComponent* GetSpringArmComponent() const { return SpringArmComponent; }

	/** Get player camera component */
	UFUNCTION(BlueprintPure, Category = "Player|Components")
	UCameraComponent* GetCameraComponent() const { return CameraComponent; }

	/** Get health component */
	UFUNCTION(BlueprintPure, Category = "Player|Components")
	UUnitHealthComponent* GetHealthComponent() const { return HealthComponent; }

	/** Get component that deal logic of how player react to hits */
	UFUNCTION(BlueprintPure, Category = "Player|Components")
	UHitAnimationDirectorComponent* GetHitAnimationDirectorComponent() const { return HitAnimationDirectorComponent; }

	/** Get base anim instance of player */
	UFUNCTION(BlueprintPure, Category = "Player|Components")
	UAdvancedAnimInstance* GetAnimInstance() const { return AnimInstance; }

	/** Get current playing hit montage animation */
	UFUNCTION(BlueprintPure, Category = "Player|Behavior")
	UAnimMontage* GetCurrentHitAnimation() const { return CurrentHitAnimation; }

	/** Get Class of camera shake effect, used when player take damage */
	UFUNCTION(BlueprintPure, Category = "Player|FX")
	TSubclassOf<UCameraShakeBase> GetCameraShakeClass() const { return CameraShakeClass; }

	/** Get maximum character walking speed */
	UFUNCTION(BlueprintPure, Category = "Player|Movement")
	float GetCharacterWalkingSpeed() const;

	/** Get value of character moving stamina */
	UFUNCTION(BlueprintPure, Category = "Player|Movement")
	int GetStamina() const { return Stamina; }

	/** Get manna reserve for magical attacks */
	UFUNCTION(BlueprintPure, Category = "Player|Behavior")
	int GetManna() const { return Manna; }

	/** Get is player focus someone or not */
	UFUNCTION(BlueprintPure, Category = "Player|Behavior")
	bool GetInFocus() const { return bInFocus; }

	/** Add amount of manna for magical attacks */
	UFUNCTION(BlueprintCallable, Category = "Player|Behavior")
	void AddManna(int Amount);

	/** Set maximum character walking speed */
	UFUNCTION(BlueprintCallable, Category = "Player|Movement")
	void SetCharacterWalkingSpeed(float Speed);

	/** Set class of camera shake effect, used when player take damage */
	UFUNCTION(BlueprintCallable, Category = "Player|FX")
	void SetCameraShakeClass(TSubclassOf<UCameraShakeBase> CameraShakeBaseClass) { CameraShakeClass = CameraShakeBaseClass; }

	// Character Interface Implementation
	virtual ACharacter* GetCharacterSelf_Implementation() override;
	virtual USkeletalMeshComponent* GetCharacterMesh_Implementation() const override;
	virtual AWeaponBase* GetCurrentWeapon_Implementation() const override;
	virtual EUnitState GetUnitState_Implementation() const override;
	virtual EUnitType GetUnitType_Implementation() const override;
	virtual bool GetIsDead_Implementation() const override;
	virtual bool GetInvincibility_Implementation() const override;
	virtual void SetUnitState_Implementation(EUnitState NewUnitState) override;
	virtual void SetInvincibility_Implementation(bool bSetInvisible) override;
	virtual void SetCurrentWeapon_Implementation(AWeaponBase* Weapon) override;
	// End of Character Interface Implementation
};
