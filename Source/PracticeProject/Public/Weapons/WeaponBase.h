// Copyright 2023 Vsevolod Khlebnikov. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/WeaponInterface.h"
#include "UObject/Object.h"
#include "Weapons/Components/DamageProviderComponent.h"
#include "Weapons/Components/MovesetDirectorComponent.h"
#include "Weapons/Enums/EWeaponSubType.h"
#include "Weapons/Enums/EWeaponType.h"

#include "WeaponBase.generated.h"

/**
 * Base class for weapon
 */
UCLASS()
class PRACTICEPROJECT_API AWeaponBase : public AActor, public IWeaponInterface
{
	GENERATED_BODY()

private:
	/** Component that parse configs with attacks data, and select combo for weapon owner */
	UPROPERTY(EditAnywhere, Category = "Component")
	UMovesetDirectorComponent* MovesetDirectorComponent;

	/** Component that provide and deal damage to actor that would be hit by this weapon */
	UPROPERTY(EditAnywhere, Category = "Component")
	UDamageProviderComponent* DamageProviderComponent;

	/** Component that serialize and deserialize combo attacks for units from .json configure */
	UPROPERTY(EditAnywhere, Category = "Component")
	UJsonSerializerComponent* JsonSerializerComponent;

	/** Name of the current weapon */
	UPROPERTY(EditAnywhere, Category = "Settings")
	FName WeaponName;

	/** Certain type of the weapon */
	UPROPERTY(EditAnywhere, Category = "Settings")
	EWeaponType WeaponType;

	/** Sub type of the weapon */
	UPROPERTY(EditAnywhere, Category = "Settings")
	EWeaponSubType WeaponSubType;

	/** Additional damage multiplier of the weapon */
	UPROPERTY(EditAnywhere, Category = "Settings")
	float WeaponDamageRate;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Sets default values for this actor's properties
	AWeaponBase();

	/** Get weapon moveset director component */
	UFUNCTION(BlueprintPure, Category = "Weapon Base")
	UMovesetDirectorComponent* GetMovesetDirectorComponent() const { return MovesetDirectorComponent; }

	/** Get weapon damage provider component */
	UFUNCTION(BlueprintPure, Category = "Weapon Base")
	UDamageProviderComponent* GetDamageProviderComponent() const { return DamageProviderComponent; }

	/** Get json serializer component */
	UFUNCTION(BlueprintPure, Category = "Moveset Director Component")
	UJsonSerializerComponent* GetJsonSerializerComponent() const { return JsonSerializerComponent; }

	/** Get weapon certain type */
	UFUNCTION(BlueprintPure, Category = "Weapon Base")
	EWeaponType GetWeaponType() const { return WeaponType; }

	/** Get weapon sub type */
	UFUNCTION(BlueprintPure, Category = "Weapon Base")
	EWeaponSubType GetWeaponSubType() const { return WeaponSubType; }

	/** Get weapon damage rate */
	UFUNCTION(BlueprintPure, Category = "Weapon Base")
	float GetWeaponDamageRate() const { return WeaponDamageRate; }
};
