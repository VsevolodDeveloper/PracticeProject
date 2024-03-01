// Copyright 2023 Vsevolod Khlebnikov. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "WeaponInterface.generated.h"

UINTERFACE(MinimalAPI)
class UWeaponInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Base interface for every Practice Project weapon
 */
class PRACTICEPROJECT_API IWeaponInterface
{
	GENERATED_BODY()

public:
	/** Return weapon mesh component */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, meta = (Category = "Weapon Interface", OverrideNativeName = "GetWeaponMeshComponent"))
	UStaticMeshComponent* GetWeaponMeshComponent();
};
