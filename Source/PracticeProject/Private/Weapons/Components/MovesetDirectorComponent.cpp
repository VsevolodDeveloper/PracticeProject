// Copyright 2023 Vsevolod Khlebnikov. All Rights Reserved.

#include "Weapons/Components/MovesetDirectorComponent.h"

// Sets default values for this component's properties
UMovesetDirectorComponent::UMovesetDirectorComponent()
{
	JsonSerializerComponent = CreateDefaultSubobject<UJsonSerializerComponent>(TEXT("JsonSerializerComponent"));

	PrimaryComponentTick.bCanEverTick = true;
	HeavyAttackDamageRate = 2.5f;
	MovesetConfigsTablePath = "/Game/Blueprints/Weapons/Data/ST_MovesetConfigurations";
}

void UMovesetDirectorComponent::Initialize(const FName& WeaponName, EUnitType UnitType, EWeaponType WeaponType)
{
	WeaponOwnerUnitType = UnitType;
	OwnerWeaponType = WeaponType;

	FString UnitKeyName = UEnum::GetDisplayValueAsText(WeaponOwnerUnitType).ToString();
	UnitKeyName.RemoveSpacesInline();

	const FString ConfigKeyName = UnitKeyName.Append(WeaponName.ToString());
	FString FileName = FText::FromStringTable(MovesetConfigsTablePath, ConfigKeyName).ToString();

	JsonSerializerComponent->LoadJson(FileName);
}

UAnimMontage* UMovesetDirectorComponent::GetComboAttack(TArray<FString>& Combo, bool& bHasComboAttacks, int& StaminaCostOfSelectedCombo)
{
	const TMap<FString, UAnimMontage*> ComboAttackList = GetComboAttackList(Combo, StaminaCostOfSelectedCombo);
	if (CurrentComboAttackIndex > Combo.Num() - 1)
	{
		bHasComboAttacks = false;
		ResetCurrentComboAttackIndex();
		return nullptr;
	}
	else
	{
		bHasComboAttacks = true;
		const int CachedComboAttackIndex = CurrentComboAttackIndex;
		CurrentComboAttackIndex = CurrentComboAttackIndex + 1;
		return ComboAttackList[Combo[CachedComboAttackIndex]];
	}
}

TMap<FString, UAnimMontage*> UMovesetDirectorComponent::GetComboAttackList(TArray<FString>& ComboAttacks, int& StaminaCostOfSelectedCombo)
{
	const FString ComboName = JsonSerializerComponent->GetRandomAttackFromCombo(JsonSerializerComponent->UnitAtttacksByWeapon);
	AttackDamageRate = JsonSerializerComponent->GetDamageRateOfCombo(JsonSerializerComponent->UnitAtttacksByWeapon, ComboName);

	for (int i = 0; i < JsonSerializerComponent->UnitAtttacksByWeapon.ComboAttacks.Num(); i++)
	{
		const FComboAttackData ComboAttack = JsonSerializerComponent->UnitAtttacksByWeapon.ComboAttacks[i];
		if (ComboName == ComboAttack.ComboName)
		{
			ComboAttacks = ComboAttack.AttacksChain;
			StaminaCostOfSelectedCombo = JsonSerializerComponent->GetRequiredStaminaForCombo(JsonSerializerComponent->UnitAtttacksByWeapon, ComboName);
			return WeaponMoveset[GetWeaponOwnerUnitType()].ComboAttackAnimationMoveset;
		}
	}

	return WeaponMoveset[GetWeaponOwnerUnitType()].ComboAttackAnimationMoveset;
}

UAnimMontage* UMovesetDirectorComponent::GetHeavyAttack()
{
	AttackDamageRate = HeavyAttackDamageRate;
	const FAnimationStructure WeaponMovesetForSelectedUnit = WeaponMoveset[GetWeaponOwnerUnitType()];
	return WeaponMovesetForSelectedUnit.HeavyAttackAnimation;
}
