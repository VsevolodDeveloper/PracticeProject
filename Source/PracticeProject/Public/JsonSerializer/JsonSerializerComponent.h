// Copyright 2023 Vsevolod Khlebnikov. All Rights Reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"
#include "GenericPlatform/GenericPlatformFile.h"
#include "HAL/PlatformFilemanager.h"
#include "JsonSerializer/Structs/FComboAttackData.h"
#include "JsonSerializer/Structs/FUnitAttacksByWeapon.h"
#include "Runtime/JsonUtilities/Public/JsonObjectConverter.h"

#include "JsonSerializerComponent.generated.h"

/**
 * Component that serialize and deserialize combo attacks for units from .json configure
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PRACTICEPROJECT_API UJsonSerializerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UJsonSerializerComponent();

	/** Hold configure of unit attacks by certain weapon, should be deserialized from .json */
	UPROPERTY(BlueprintReadWrite, Category = "JsonComponent")
	FUnitAttacksByWeapon UnitAtttacksByWeapon;

	/** Deserialize configure from .json file */
	UFUNCTION(BlueprintCallable, Category = "JsonComponent")
	bool LoadJson(FString FileName);

	/** Serialize configure to .json file */
	UFUNCTION(BlueprintCallable, Category = "JsonComponent")
	void WriteConfig(FUnitAttacksByWeapon UnitAttacksByWeapon, FString FileName);

	/** Get random combo attack from loaded configure */
	UFUNCTION(BlueprintCallable, Category = "JsonComponent")
	FString GetRandomAttackFromCombo(FUnitAttacksByWeapon UnitAttacksByWeapon);

	/** Get damage rate of combo attack */
	UFUNCTION(BlueprintCallable, Category = "JsonComponent")
	float GetDamageRateOfCombo(FUnitAttacksByWeapon UnitAttacksByWeapon, FString ComboName);

	/** Get required stamina for combo attack */
	UFUNCTION(BlueprintCallable, Category = "JsonComponent")
	int GetRequiredStaminaForCombo(FUnitAttacksByWeapon UnitAttacksByWeapon, FString ComboName);

	/** Get combo structure elements in string type */
	UFUNCTION(BlueprintCallable, Category = "JsonComponent")
	FString GetComboDataInString(FUnitAttacksByWeapon UnitAttacksByWeapon);
};
