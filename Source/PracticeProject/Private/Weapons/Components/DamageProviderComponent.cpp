// Copyright 2023 Vsevolod Khlebnikov. All Rights Reserved.

#include "Weapons/Components/DamageProviderComponent.h"
#include "Interfaces/CharacterInterface.h"
#include "PracticeProject/PracticeProjectCustomLogs.h"

DEFINE_LOG_CATEGORY(DamageProviderComponent_LOG);

// Sets default values for this component's properties
UDamageProviderComponent::UDamageProviderComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	MinDamageMultiplierOnRecoveryHit = 0.4f;
	MaxDamageMultiplierOnRecoveryHit = 0.5f;
	MinDamageMultiplierOnCharacterHit = 0.9f;
	MaxDamageMultiplierOnCharacterHit = 1.1f;

	HitImpulseStrength = 1000.0f;
	bCanHit = true;

	EnablePhysicalRecoveryMethodName = "RecoverySetHitPhysics";
}

void UDamageProviderComponent::DealHit(const FHitResult& Hit, bool IsHit, float AttackDamageRate, float WeaponDamageRate, AActor* DamageCauser)
{
	SetCanHitActor(!IsHit);
	const float AccumulatedDamage = AttackDamageRate * WeaponDamageRate;
	if (IsHit)
	{
		ApplyHit(DamageCauser, Hit, AccumulatedDamage);
	}
}

void UDamageProviderComponent::ApplyHit(AActor* DamageCauser, const FHitResult& Hit, float AccumulatedDamage)
{
	if (IsValid(Hit.GetActor()))
	{
		if (IsHitTargetIsCharacter(Hit))
		{
			TryToHitCharacterOrHisComponents(DamageCauser, Hit, AccumulatedDamage);
		}
		else
		{
			TryToHitComponent(Hit, DamageCauser, AccumulatedDamage);
		}
	}
}

void UDamageProviderComponent::TryToHitComponent(const FHitResult& Hit, AActor* DamageCauser, float AccumulatedDamage)
{
	if (IsRecoveryComponentWasHitted(Hit))
	{
		ApplyHitToRecoveryComponent(DamageCauser, AccumulatedDamage, Hit);
	}
	else if (IsComponentCanBeDamaged(Hit))
	{
		ApplyHitToOtherComponent(Hit, AccumulatedDamage, DamageCauser);
	}
	else
	{
		SetCanHitActor(true);
	}
}

void UDamageProviderComponent::TryToHitCharacterOrHisComponents(AActor* DamageCauser, const FHitResult& Hit, float AccumulatedDamage)
{
	if (IsHitTargetAndDamageCauserBothCharacters(DamageCauser, Hit))
	{
		if (IsHitTargetAndDamageCauserBothEnemies(DamageCauser, Hit))
		{
			SetCanHitActor(true);
		}
		else
		{
			if (IsRecoveryComponentWasHitted(Hit))
			{
				ApplyHitToRecoveryComponent(DamageCauser, AccumulatedDamage, Hit);
			}
			else
			{
				ApplyHitToCharacter(Hit, AccumulatedDamage, DamageCauser);
			}
		}
	}
}

void UDamageProviderComponent::ApplyHitToCharacter(const FHitResult& Hit, float AccumulatedDamage, AActor* DamageCauser)
{
	ICharacterInterface* CharacterInterface = Cast<ICharacterInterface>(Hit.GetActor());
	USkeletalMeshComponent* MeshOfHittedActor = CharacterInterface->Execute_GetCharacterMesh(Hit.GetActor());

	if (MeshOfHittedActor->IsSimulatingPhysics(Hit.BoneName))
	{
		MeshOfHittedActor->AddImpulseAtLocation(FVector(HitImpulseStrength), Hit.Location, Hit.BoneName);
	}

	SpawnBloodParticles(Hit);

	float InaccurateDamage = UKismetMathLibrary::RandomFloatInRange(AccumulatedDamage * MinDamageMultiplierOnCharacterHit, AccumulatedDamage * MaxDamageMultiplierOnCharacterHit);
	UGameplayStatics::ApplyDamage(Hit.GetActor(), InaccurateDamage, nullptr, DamageCauser, nullptr);
}

void UDamageProviderComponent::ApplyHitToRecoveryComponent(AActor* DamageCauser, float AccumulatedDamage, const FHitResult& Hit)
{
	SetCanHitActor(false);

	float InaccurateDamage = UKismetMathLibrary::RandomFloatInRange(AccumulatedDamage * MinDamageMultiplierOnRecoveryHit, AccumulatedDamage * MaxDamageMultiplierOnRecoveryHit);
	UGameplayStatics::ApplyDamage(Hit.GetActor(), InaccurateDamage, nullptr, DamageCauser, nullptr);

	SpawnRecoverysSparkles(DamageCauser, Hit);

	if (Hit.GetComponent()->GetOwner()->FindComponentByClass(RecoveryComponentClass))
	{
		FOutputDeviceNull Device;
		Hit.GetComponent()->GetOwner()->GetComponentByClass(RecoveryComponentClass)->CallFunctionByNameWithArguments(*EnablePhysicalRecoveryMethodName, Device, NULL, true);
	}
}

void UDamageProviderComponent::ApplyHitToOtherComponent(const FHitResult& Hit, float AccumulatedDamage, AActor* DamageCauser)
{
	SetCanHitActor(false);
	UGameplayStatics::ApplyDamage(Hit.GetActor(), AccumulatedDamage, DamageCauser->GetInstigatorController(), DamageCauser, UDamageType::StaticClass());
}

bool UDamageProviderComponent::IsHitTargetIsCharacter(const FHitResult& Hit) const
{
	return Hit.GetActor()->GetClass()->ImplementsInterface(UCharacterInterface::StaticClass());
}

bool UDamageProviderComponent::IsComponentCanBeDamaged(const FHitResult& Hit) const
{
	return Hit.GetComponent()->ComponentHasTag("CanBeDamaged");
}

bool UDamageProviderComponent::IsRecoveryComponentWasHitted(const FHitResult& Hit) const
{
	return Hit.GetComponent()->ComponentHasTag("Recovery");
}

bool UDamageProviderComponent::IsHitTargetAndDamageCauserBothEnemies(AActor* DamageCauser, const FHitResult& Hit) const
{
	return DamageCauser->ActorHasTag("Enemy") && Hit.GetActor()->ActorHasTag("Enemy");
}

bool UDamageProviderComponent::IsHitTargetAndDamageCauserBothCharacters(AActor* DamageCauser, const FHitResult& Hit) const
{
	ICharacterInterface* CharacterInterface = Cast<ICharacterInterface>(Hit.GetActor());
	const bool bHittedActorDead = CharacterInterface->Execute_GetIsDead(Hit.GetActor());
	const bool bHittedActorInInvincibility = CharacterInterface->Execute_GetInvincibility(Hit.GetActor());
	const bool bDamageCauserIsCharacter = DamageCauser->ActorHasTag("Character");
	const bool bHittedActorIsCharacter = Hit.GetActor()->ActorHasTag("Character");
	return ((bDamageCauserIsCharacter && bHittedActorIsCharacter) && (!bHittedActorDead && !bHittedActorInInvincibility));
}

void UDamageProviderComponent::SpawnBloodParticles(const FHitResult& Hit)
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BloodEmitter, Hit.Location, FRotator(0.0f), FVector(0.2f));
}

void UDamageProviderComponent::SpawnRecoverysSparkles(AActor* HitCauser, const FHitResult& Hit)
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), RecoverysSparklesEmitter, Hit.Location, FRotator(0.0f), FVector(0.2f));
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), RecoveryHitSound, Hit.Location, FRotator::ZeroRotator, 1.0f, 1.0f, 0.0f, AttenuationSettings, nullptr, HitCauser);
}
