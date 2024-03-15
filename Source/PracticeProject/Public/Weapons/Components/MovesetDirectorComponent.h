// Copyright 2023 Vsevolod Khlebnikov. All Rights Reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"
#include "Internationalization/Text.h"
#include "JsonSerializer/JsonSerializerComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Units/Enums/EUnitType.h"
#include "Weapons/Enums/EWeaponType.h"
#include "Weapons/Structs/FAttackAnimations.h"

#include "MovesetDirectorComponent.generated.h"

/**
 *  Component that parse configs with attacks data, and select combo
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PRACTICEPROJECT_API UMovesetDirectorComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	/** Pointer to weapon component that serialize and deserialize combo attacks for units from .json configure */
	UPROPERTY(Transient)
	UJsonSerializerComponent* JsonSerializerComponent;

	/** Animation moveset for different type of units that can use weapon */
	UPROPERTY(EditDefaultsOnly, Category = "Behavior")
	TMap<EUnitType, FAnimationStructure> WeaponMoveset;

	/** Name of the table where all moveset configurations are stored */
	UPROPERTY(EditDefaultsOnly, Category = "Behavior")
	FName MovesetConfigsTablePath;

	/** Damage multiplier for heavy attack */
	UPROPERTY(EditDefaultsOnly, Category = "Behavior")
	float HeavyAttackDamageRate;

	/** Damage multiplier for current attack */
	float AttackDamageRate;

	/** Cached index of current attack from selected combo */
	int CurrentComboAttackIndex;

	/** Weapon type of component owner */
	EWeaponType OwnerWeaponType;

	/** Unit type of weapon owner */
	EUnitType WeaponOwnerUnitType;

public:
	// Sets default values for this component's properties
	UMovesetDirectorComponent();

	/** Initialize settings and load json file */
	UFUNCTION(BlueprintCallable, Category = "Moveset Director Component")
	void Initialize(UJsonSerializerComponent* InJsonSerializerComponent, const FName& WeaponName, EUnitType UnitType, EWeaponType WeaponType);

	/** Get Combo attack animations list */
	UFUNCTION(BlueprintCallable, Category = "Moveset Director Component")
	TMap<FString, UAnimMontage*> GetComboAttackList(TArray<FString>& ComboAttacks, int& StaminaCostOfSelectedCombo);

	/** Create and initialize combo attack data and return combo attack animation */
	UFUNCTION(BlueprintCallable, Category = "Moveset Director Component")
	UAnimMontage* GetComboAttack(TArray<FString>& Combo, bool& bHasComboAttacks, int& StaminaCostOfSelectedCombo);

	/** Get animation of heavy attack */
	UFUNCTION(BlueprintCallable, Category = "Moveset Director Component")
	UAnimMontage* GetHeavyAttack();

	/** Get weapon moveset */
	UFUNCTION(BlueprintPure, Category = "Moveset Director Component")
	TMap<EUnitType, FAnimationStructure> GetWeaponMoveset() const { return WeaponMoveset; }

	/** Get current attack damage rate */
	float GetAttackDamageRate() const { return AttackDamageRate; }

	/** Get unit type of weapon owner */
	EUnitType GetWeaponOwnerUnitType() const { return WeaponOwnerUnitType; }

	/** Get weapon type of component owner */
	EWeaponType GetWeaponType() const { return OwnerWeaponType; }

	/** Invalidate Cached index of current attack from current combo*/
	UFUNCTION(BlueprintCallable, Category = "Moveset Director Component")
	void ResetCurrentComboAttackIndex() { CurrentComboAttackIndex = 0; }
};
