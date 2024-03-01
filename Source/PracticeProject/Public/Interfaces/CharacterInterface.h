// Copyright 2023 Vsevolod Khlebnikov. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Units/Enums/EUnitState.h"
#include "Units/Enums/EUnitType.h"
#include "Weapons/WeaponBase.h"

#include "CharacterInterface.generated.h"

UINTERFACE(MinimalAPI)
class UCharacterInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Base Interface for every Practice Project character
 */
class PRACTICEPROJECT_API ICharacterInterface
{
	GENERATED_BODY()

public:
	/** Return character itself */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, meta = (Category = "Character Interface", OverrideNativeName = "GetCharacter", DisplayName = "GetCharacter"))
	ACharacter* GetCharacterSelf();

	/** Return character mesh */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, meta = (Category = "Character Interface", OverrideNativeName = "GetMesh"))
	USkeletalMeshComponent* GetCharacterMesh() const;

	/** Return weapon that character use right now  */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, meta = (Category = "Character Interface", OverrideNativeName = "GetCurrentWeapon"))
	AWeaponBase* GetCurrentWeapon() const;

	/** Return unit type */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, meta = (Category = "Character Interface", OverrideNativeName = "GetUnitType"))
	EUnitType GetUnitType() const;

	/** Return unit state */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, meta = (Category = "Character Interface", OverrideNativeName = "GetUnitState"))
	EUnitState GetUnitState() const;

	/** Return is unit dead or not */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, meta = (Category = "Character Interface", OverrideNativeName = "GetIsDead"))
	bool GetIsDead() const;

	/** Return is unit can be hit or not */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, meta = (Category = "Character Interface", OverrideNativeName = "GetInvincibility"))
	bool GetInvincibility() const;

	/** Set unit type */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, meta = (Category = "Character Interface", OverrideNativeName = "SetUnitState"))
	void SetUnitState(EUnitState NewUnitState);

	/** Set is unit can be hit or not */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, meta = (Category = "Character Interface", OverrideNativeName = "SetInvincibility"))
	void SetInvincibility(bool bSetInvisible);

	/** Set weapon that character use right now */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, meta = (Category = "Character Interface", OverrideNativeName = "SetCurrentWeapon"))
	void SetCurrentWeapon(AWeaponBase* Weapon);
};
