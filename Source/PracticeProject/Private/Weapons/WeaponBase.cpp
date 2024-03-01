// Copyright 2023 Vsevolod Khlebnikov. All Rights Reserved.

#include "Weapons/WeaponBase.h"
#include "Interfaces/CharacterInterface.h"

// Sets default values
AWeaponBase::AWeaponBase()
{
	MovesetDirectorComponent = CreateDefaultSubobject<UMovesetDirectorComponent>(TEXT("AnimationMovesetComp"));
	DamageProviderComponent = CreateDefaultSubobject<UDamageProviderComponent>(TEXT("DamageProviderComponent"));

	WeaponDamageRate = 1.0f;

	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();

	if (AWeaponBase::GetOwner()->GetClass()->ImplementsInterface(UCharacterInterface::StaticClass()))
	{
		const ICharacterInterface* Interface = Cast<ICharacterInterface>(AWeaponBase::GetOwner());
		const EUnitType OwnerUnitType = Interface->Execute_GetUnitType(AWeaponBase::GetOwner());
		MovesetDirectorComponent->Initialize(WeaponName, OwnerUnitType, GetWeaponType());
	}
}