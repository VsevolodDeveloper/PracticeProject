// Copyright 2023 Vsevolod Khlebnikov. All Rights Reserved.

#include "JsonSerializer/JsonSerializerComponent.h"
#include "PracticeProject/PracticeProjectCustomLogs.h"

DEFINE_LOG_CATEGORY(JsonComponent_LOG);

UJsonSerializerComponent::UJsonSerializerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	ConfigsPath = FPaths::ProjectConfigDir() + TEXT("MovesetConfigs/");
}

FString UJsonSerializerComponent::GetComboDataInString(FUnitAttacksByWeapon UnitAttacksByWeapon)
{
	FString ComboDataInString = "";

	ComboDataInString += "unit_name: " + UnitAttacksByWeapon.UnitName + "\r\n";
	ComboDataInString += "weapon_name: " + UnitAttacksByWeapon.WeaponName + "\r\n";

	ComboDataInString += "combo_attacks:";
	ComboDataInString += "\r\n";

	for (int32 i = 0; i < UnitAttacksByWeapon.ComboAttacks.Num(); i++)
	{
		ComboDataInString += "combo_name: " + UnitAttacksByWeapon.ComboAttacks[i].ComboName + "\r\n";
		ComboDataInString += "required_stamina: " + FString::FromInt(UnitAttacksByWeapon.ComboAttacks[i].RequiredStamina) + "\r\n";

		ComboDataInString += "attacks_chain:";
		ComboDataInString += "\r\n";

		for (int32 j = 0; j < UnitAttacksByWeapon.ComboAttacks[i].AttacksChain.Num(); j++)
		{
			ComboDataInString += "attack_name: " + UnitAttacksByWeapon.ComboAttacks[i].AttacksChain[j] + ", ";
		}

		ComboDataInString += "damage_rate: " + FString::SanitizeFloat(UnitAttacksByWeapon.ComboAttacks[i].DamageRate) + "\r\n";
	}

	return ComboDataInString;
}

FString UJsonSerializerComponent::GetRandomAttackFromCombo(FUnitAttacksByWeapon UnitAttacksByWeapon)
{
	 return UnitAttacksByWeapon.ComboAttacks[rand() % (UnitAttacksByWeapon.ComboAttacks.Num())].ComboName;
}

float UJsonSerializerComponent::GetDamageRateOfCombo(FUnitAttacksByWeapon UnitAttacksByWeapon, FString ComboName)
{
	for (int32 i = 0; i < UnitAttacksByWeapon.ComboAttacks.Num(); i++)
	{
		if (UnitAttacksByWeapon.ComboAttacks[i].ComboName == ComboName)
		{
			return UnitAttacksByWeapon.ComboAttacks[i].DamageRate;
		}
	}

	UE_LOG(JsonComponent_LOG, Warning, TEXT("Could not find current combo"));
	return 0.0f;
}

int UJsonSerializerComponent::GetRequiredStaminaForCombo(FUnitAttacksByWeapon UnitAttacksByWeapon, FString ComboName)
{
	for (int32 i = 0; i < UnitAttacksByWeapon.ComboAttacks.Num(); i++)
	{
		if (UnitAttacksByWeapon.ComboAttacks[i].ComboName == ComboName)
		{
			return UnitAttacksByWeapon.ComboAttacks[i].RequiredStamina;
		}
	}

	UE_LOG(JsonComponent_LOG, Warning, TEXT("Could not find current combo"));
	return 0;
}


void UJsonSerializerComponent::WriteConfig(FUnitAttacksByWeapon UnitAttacksByWeapon, FString FileName)
{
	TSharedPtr<FJsonObject> ConfigObject = MakeShareable(new FJsonObject);
	TSharedPtr<FJsonObject> UnitObject = MakeShareable(new FJsonObject);

	ConfigObject->SetObjectField("unit_attacks_by_weapon", UnitObject);
	UnitObject->SetStringField("unit_name", UnitAttacksByWeapon.UnitName);
	UnitObject->SetStringField("weapon_name", UnitAttacksByWeapon.WeaponName);

	TArray<TSharedPtr<FJsonValue>> ComboAttacks;

	for (int32 i = 0; i < UnitAttacksByWeapon.ComboAttacks.Num(); i++)
	{
		TSharedPtr<FJsonObject> AttackObject = MakeShareable(new FJsonObject);
		AttackObject->SetStringField("combo_name", UnitAttacksByWeapon.ComboAttacks[i].ComboName);
		AttackObject->SetNumberField("required_stamina", UnitAttacksByWeapon.ComboAttacks[i].RequiredStamina);

		TArray<FString> StringOfAttacksChain = UnitAttacksByWeapon.ComboAttacks[i].AttacksChain;
		TArray<TSharedPtr<FJsonValue>> AttacksChain;

		for (const auto& Attack : StringOfAttacksChain)
		{
			AttacksChain.Add(MakeShareable(new FJsonValueString(Attack)));
		}

		AttackObject->SetArrayField("attacks_chain", AttacksChain);

		AttackObject->SetNumberField("damage_rate", UnitAttacksByWeapon.ComboAttacks[i].DamageRate);
		ComboAttacks.Add(MakeShareable(new FJsonValueObject(AttackObject)));
	}

	UnitObject->SetArrayField("combo_attacks", ComboAttacks);

	FString ConfigStream;
	TSharedRef<TJsonWriter<TCHAR>> JsonWriter = TJsonWriterFactory<TCHAR>::Create(&ConfigStream);
	FJsonSerializer::Serialize(ConfigObject.ToSharedRef(), JsonWriter);

	const FString FullPath = ConfigsPath + FileName;
	FFileHelper::SaveStringToFile(ConfigStream, *FullPath);
}

bool UJsonSerializerComponent::LoadJson(FString FileName)
{
	const FString FullPath = ConfigsPath + FileName;

	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*FullPath))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Could not find file"));
		UE_LOG(JsonComponent_LOG, Warning, TEXT("Could not find file"));
		return false;
	}
	else
	{
		FString ConfigStream;
		FFileHelper::LoadFileToString(ConfigStream, *FullPath);

		TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(ConfigStream);
		TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
		FJsonSerializer::Deserialize(JsonReader, JsonObject);

		TSharedPtr<FJsonObject> UnitObject = JsonObject->GetObjectField("unit_attacks_by_weapon");

		UnitAtttacksByWeapon.UnitName = UnitObject->GetStringField("unit_name");
		UnitAtttacksByWeapon.WeaponName = UnitObject->GetStringField("weapon_name");

		TArray<TSharedPtr<FJsonValue>> ComboAttacks;
		ComboAttacks = UnitObject->GetArrayField("combo_attacks");

		for (int32 i = 0; i < ComboAttacks.Num(); i++)
		{
			FComboAttackData Combo;
			Combo.ComboName = ComboAttacks[i]->AsObject()->GetStringField("combo_name");
			Combo.RequiredStamina = ComboAttacks[i]->AsObject()->GetIntegerField("required_stamina");

			TArray<TSharedPtr<FJsonValue>> AttacksChain;
			AttacksChain = ComboAttacks[i]->AsObject()->GetArrayField("attacks_chain");

			for (const auto& Attack : AttacksChain)
			{
				FString ArrayElement = Attack->AsString();
				Combo.AttacksChain.Add(ArrayElement);
			}

			Combo.DamageRate = ComboAttacks[i]->AsObject()->GetNumberField("damage_rate");

			UnitAtttacksByWeapon.ComboAttacks.Add(Combo);
		}
		return true;
	}
}