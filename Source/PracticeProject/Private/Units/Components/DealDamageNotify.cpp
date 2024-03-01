// Copyright 2023 Vsevolod Khlebnikov. All Rights Reserved.

#include "Units/Components/DealDamageNotify.h"
#include "PracticeProject/PracticeProjectCustomLogs.h"

DEFINE_LOG_CATEGORY(DealDamageNotify_LOG);

UDealDamageNotify::UDealDamageNotify()
{
	TraceHalfSize = FVector(2.0f);
	TraceTime = 2.0f;
}

void UDealDamageNotify::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	UAnimNotifyState::GetNotifyName() = "Hit Window";
}

void UDealDamageNotify::NotifyEnd(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation)
{
	if (MeshComp->GetOwner()->GetClass()->ImplementsInterface(UCharacterInterface::StaticClass()))
	{
		AWeaponBase* Weapon = GetOwnerWeapon(MeshComp);
		Weapon->GetDamageProviderComponent()->SetCanHitActor(true);
	}
}

void UDealDamageNotify::NotifyTick(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	if (MeshComp->GetOwner()->GetClass()->ImplementsInterface(UCharacterInterface::StaticClass()))
	{
		AWeaponBase* Weapon = GetOwnerWeapon(MeshComp);
		if (Weapon->GetClass()->ImplementsInterface(UWeaponInterface::StaticClass()))
		{
			if (Weapon->GetDamageProviderComponent()->GetCanHitActor())
			{
				FHitResult Hit;
				const bool IsHit = TraceForDamageHit(MeshComp, GetWeaponMesh(Weapon), Hit);

				const float WeaponDamageRate = Weapon->GetWeaponDamageRate();
				const float AttackDamageRate = Weapon->GetMovesetDirectorComponent()->GetAttackDamageRate();

				Weapon->GetDamageProviderComponent()->DealHit(Hit, IsHit, AttackDamageRate, WeaponDamageRate, MeshComp->GetOwner());
			}
		}
	}
}

AWeaponBase* UDealDamageNotify::GetOwnerWeapon(class USkeletalMeshComponent* MeshComponent) const
{
	ICharacterInterface* CharacterInterface = Cast<ICharacterInterface>(MeshComponent->GetOwner());
	return CharacterInterface->Execute_GetCurrentWeapon(MeshComponent->GetOwner());
}

UStaticMeshComponent* UDealDamageNotify::GetWeaponMesh(AWeaponBase* GivenWeapon) const
{
	IWeaponInterface* WeaponInterface = Cast<IWeaponInterface>(GivenWeapon);
	return WeaponInterface->Execute_GetWeaponMeshComponent(GivenWeapon);
}

bool UDealDamageNotify::TraceForDamageHit(USkeletalMeshComponent* CharacterMesh, UStaticMeshComponent* WeaponMesh, FHitResult& Hit)
{
	const FVector TraceStartLocation = WeaponMesh->GetSocketLocation("WeaponTop");
	const FVector TraceEndLocation = WeaponMesh->GetSocketLocation("WeaponBottom");
	return UKismetSystemLibrary::BoxTraceSingle(CharacterMesh, TraceStartLocation, TraceEndLocation, TraceHalfSize, FRotator::ZeroRotator, ETraceTypeQuery::TraceTypeQuery2, false, {}, EDrawDebugTrace::None, Hit, true, FColor::Red, FColor::Green, TraceTime);
}