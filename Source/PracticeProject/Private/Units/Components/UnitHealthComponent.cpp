// Copyright 2023 Vsevolod Khlebnikov. All Rights Reserved.

#include "Units/Components/UnitHealthComponent.h"
#include "GameFramework/Actor.h"

// Sets default values for this component's properties
UUnitHealthComponent::UUnitHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetInitialHealth(100.0f);
	bCallAutomatically = true;
}

// Called when the game starts
void UUnitHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* Owner = GetOwner();
	if (Owner)
	{
		SetHealth(InitialHealth);

		if (bCallAutomatically)
		{
			Owner->OnTakeAnyDamage.AddUniqueDynamic(this, &UUnitHealthComponent::TakeDamage);
		}
	}
}

void UUnitHealthComponent::TakeDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage >= 0.0f)
	{
		SetHealth(FMath::Clamp(GetHealth() - Damage, 0.0f, InitialHealth));
	}
}
