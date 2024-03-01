// Copyright 2023 Vsevolod Khlebnikov. All Rights Reserved.

#include "Units/EnemyBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/PlayerCharacter.h"
#include "PracticeProject/PracticeProjectCustomLogs.h"
#include "TimerManager.h"

DEFINE_LOG_CATEGORY(Enemy_LOG);

AEnemyBase::AEnemyBase()
{
	HitAnimationDirectorComponent = CreateDefaultSubobject<UHitAnimationDirectorComponent>(TEXT("HitLogicComponent"));
	PrimaryActorTick.bCanEverTick = true;

	GetCharacterMovement()->JumpZVelocity = 450.f;
	GetCharacterMovement()->MaxWalkSpeed = 150.f;
	GetCharacterMovement()->AirControl = 0.2f;

	UnitState = EUnitState::Walking;
	UnitType = EUnitType::DefaultEnemy;

	HealthTresholdForDeath = 0.1f;
	EnemyMassScaleOnDeath = 7.0f;
	RagdollInitialDelay = 0.45f;
	DeathSoundVolume = 0.3f;
	DyingTime = 10.0f;

	bCanSeePlayer = false;
	bInvincibility = false;
	bDead = false;
}

void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	if (Cast<UAdvancedAnimInstance>(GetMesh()->GetAnimInstance()))
	{
		AnimInstance = Cast<UAdvancedAnimInstance>(GetMesh()->GetAnimInstance());
		GetAnimInstance()->SetOwner(this);
	}
}

void AEnemyBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AEnemyBase::HeavyAttack()
{
	if (UnitState == EUnitState::Walking)
	{
		UAnimMontage* HeavyAttackMontage = CurrentWeapon->GetMovesetDirectorComponent()->GetHeavyAttack();
		CurrentUnitMontage = HeavyAttackMontage;

		// clang-format off
		GetAnimInstance()->ClearAllMontageCallbacks();
		GetAnimInstance()->OnMontageInterruptedCallback.AddUniqueDynamic(this, &AEnemyBase::HeavyAttackOnInterrupted);
		GetAnimInstance()->OnMontageCompletedCallback.AddUniqueDynamic(this, &AEnemyBase::HeavyAttackOnCompleted);
		GetAnimInstance()->OnMontageNotifyBeginCallback.AddUniqueDynamic(this, &AEnemyBase::HeavyAttackOnBeginNotify);
		GetAnimInstance()->OnMontageNotifyEndCallback.AddUniqueDynamic(this, &AEnemyBase::HeavyAttackOnEndNotify);
		GetAnimInstance()->PlayAnimationMontage(HeavyAttackMontage);
		// clang-format on

		Execute_SetUnitState(this, EUnitState::HeavyAttack);
	}
}

void AEnemyBase::HeavyAttackOnBeginNotify_Implementation()
{
	InvalidateCurrentMontage();
}

void AEnemyBase::HeavyAttackOnEndNotify_Implementation()
{
}

void AEnemyBase::HeavyAttackOnInterrupted_Implementation()
{
	InvalidateCurrentMontage();
	if (UnitState == EUnitState::HeavyAttack)
	{
		Execute_SetUnitState(this, EUnitState::Walking);
	}
}

void AEnemyBase::HeavyAttackOnCompleted_Implementation()
{
	InvalidateCurrentMontage();
	if (UnitState == EUnitState::HeavyAttack)
	{
		Execute_SetUnitState(this, EUnitState::Walking);
	}
}

void AEnemyBase::ComboAttack()
{
	if (UnitState == EUnitState::Stan || UnitState == EUnitState::Death)
	{
		FinishComboAttack();
	}
	else
	{
		int WastedStamina = 0; // Enemy units for now don't need stamina for attacking
		UAnimMontage* ComboAttackMontage = CurrentWeapon->GetMovesetDirectorComponent()->GetComboAttack(CurrentCombo, bHasComboAttacks, WastedStamina);

		if (HasComboAttacks() == true && ComboAttackMontage)
		{
			// clang-format off
			CurrentUnitMontage = ComboAttackMontage;
			GetAnimInstance()->ClearAllMontageCallbacks();
			GetAnimInstance()->OnMontageInterruptedCallback.AddUniqueDynamic(this, &AEnemyBase::ComboAttackOnInterrupted);
			GetAnimInstance()->OnMontageCompletedCallback.AddUniqueDynamic(this, &AEnemyBase::ComboAttackOnCompleted);
			GetAnimInstance()->OnMontageNotifyBeginCallback.AddUniqueDynamic(this, &AEnemyBase::ComboAttackOnBeginNotify);
			GetAnimInstance()->OnMontageNotifyEndCallback.AddUniqueDynamic(this, &AEnemyBase::ComboAttackOnEndNotify);
			GetAnimInstance()->PlayAnimationMontage(ComboAttackMontage);
			// clang-format on

			Execute_SetUnitState(this, EUnitState::ComboAttack);
		}
		else
		{
			FinishComboAttack();
		}
	}
}

void AEnemyBase::RandomComboAttack()
{
	if (UnitState == EUnitState::Walking)
	{
		bHasComboAttacks = true;
		ComboAttack();
	}
}

void AEnemyBase::FinishComboAttack()
{
	InvalidateCurrentMontage();
	CurrentCombo.Empty();
	CurrentWeapon->GetMovesetDirectorComponent()->ResetCurrentComboAttackIndex();
	if (UnitState == EUnitState::ComboAttack)
	{
		Execute_SetUnitState(this, EUnitState::Walking);
	}
}

void AEnemyBase::ComboAttackOnBeginNotify_Implementation()
{
	bUseControllerRotationYaw = true;
    InvalidateCurrentMontage();
}

void AEnemyBase::ComboAttackOnEndNotify_Implementation()
{
	bUseControllerRotationYaw = false;
	InvalidateCurrentMontage();
}

void AEnemyBase::ComboAttackOnCompleted_Implementation()
{
	if (UnitState == EUnitState::ComboAttack)
	{
		ComboAttack();
	}
}

void AEnemyBase::ComboAttackOnInterrupted_Implementation()
{
	InvalidateCurrentMontage();
	FinishComboAttack();
}

bool AEnemyBase::OnSeePlayer(APawn* PlayerCharacter)
{
	if (GetCanSeePlayer() == false && Cast<APlayerCharacter>(PlayerCharacter))
	{
		GetCharacterMovement()->StopActiveMovement();
		bCanSeePlayer = true;
		return true;
	}
	else
	{
		return false;
	}
}

void AEnemyBase::SetHealthBarVisibility(AActor* TargetActor, float SightRadius, UWidgetComponent* HealthBar)
{
	if (GetDistanceTo(TargetActor) > SightRadius)
	{
		HealthBar->SetWindowFocusable(true);
		HealthBar->SetVisibility(false);
	}
}

void AEnemyBase::SetEnemyWidgetsPosition(APawn* TargetCharacter, UWidgetComponent* HealthBar, UWidgetComponent* DamageBar)
{
	if (APlayerCharacter* MainPlayerCharacter = Cast<APlayerCharacter>(TargetCharacter))
	{
		const FRotator CameraRotation = MainPlayerCharacter->GetCameraComponent()->GetComponentRotation();
		const FRotator WidgetOffset = FRotator(0.0f, 0.0f, 180.0f); // turn around

		HealthBar->SetWorldRotation(CameraRotation, true, nullptr, ETeleportType::TeleportPhysics);
		HealthBar->AddWorldRotation(WidgetOffset);
		HealthBar->SetWindowFocusable(true);
		HealthBar->SetVisibility(true);

		if (IsValid(DamageBar))
		{
			DamageBar->SetWindowFocusable(true);
			DamageBar->SetWorldRotation(CameraRotation, true, nullptr, ETeleportType::TeleportPhysics);
			DamageBar->AddWorldRotation(WidgetOffset);
		}
	}
}

void AEnemyBase::OnTakeDamage(float Damage, AActor* DamageCauser, UObject* PhysicalAnimationComponent)
{
	UAnimMontage* HitMontage = GetHitAnimationDirectorComponent()->GetHitAnimation(DamageCauser, GetCapsuleComponent(), Damage, FAnimationsByDirection());

	LastTakenDamage = Damage;

	if (GetUnitHealthComponent()->GetHealth() <= HealthTresholdForDeath && bDead == false )
	{
		Death(DamageCauser, PhysicalAnimationComponent);
	}

	if (IsValid(HitMontage) && UnitState != EUnitState::Stan && bDead == false)
	{
		GetAnimInstance()->ClearAllMontageCallbacks();

		GetCharacterMovement()->StopActiveMovement();
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		StopAnimMontage(GetCurrentMontage());

		// clang-format off
		CurrentUnitMontage = HitMontage;
		GetAnimInstance()->OnMontageBlendOutCallback.AddUniqueDynamic(this, &AEnemyBase::OnTakeDamageBlendOut);
		GetAnimInstance()->OnMontageCompletedCallback.AddUniqueDynamic(this, &AEnemyBase::OnTakeDamageCompleted);
		GetAnimInstance()->OnMontageInterruptedCallback.AddUniqueDynamic(this, &AEnemyBase::OnTakeDamageInterrupted);
		GetAnimInstance()->PlayAnimationMontage(HitMontage);
		// clang-format on

		bInvincibility = true;
		bCanSeePlayer = true;

		Execute_SetUnitState(this, EUnitState::Stan);
	}
}

void AEnemyBase::OnTakeDamageBlendOut_Implementation()
{
	Execute_SetUnitState(this, EUnitState::Walking);
	bInvincibility = false;
}

void AEnemyBase::OnTakeDamageInterrupted_Implementation()
{
	Execute_SetUnitState(this, EUnitState::Walking);
	bInvincibility = false;
}

void AEnemyBase::OnTakeDamageCompleted_Implementation()
{
}

bool AEnemyBase::Death(AActor* DeathCauser, UObject* PhysicalAnimationComponent)
{
	if (bDead == false)
	{
		GetAnimInstance()->ClearAllMontageCallbacks();
		GetCharacterMovement()->StopActiveMovement();
		StopAnimMontage(GetCurrentMontage());

		bDead = true;
		Execute_SetUnitState(this, EUnitState::Death);
		UE_LOG(Enemy_LOG, Display, TEXT("Enemy %s is dead"), *this->GetName());

		UGameplayStatics::SpawnSoundAttached(DeathSound, GetMesh(), FName(TEXT("pelvis")), GetMesh()->GetRelativeLocation(), EAttachLocation::KeepRelativeOffset, false, DeathSoundVolume);

		TArray<AActor*> AttachedItems;
		this->GetAttachedActors(AttachedItems);

		//detach weapons from unit
		for (int i = 0; i < AttachedItems.Num(); i++)
		{
			AttachedItems[i]->DetachFromActor(FDetachmentTransformRules(FAttachmentTransformRules(EAttachmentRule::KeepWorld, false), false));
			if (AttachedItems[i]->GetClass()->ImplementsInterface(UWeaponInterface::StaticClass()))
			{
				IWeaponInterface* Interface = Cast<IWeaponInterface>(AttachedItems[i]);
				UStaticMeshComponent* WeaponMesh = Interface->Execute_GetWeaponMeshComponent(AttachedItems[i]);

				WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
				WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
				WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
				WeaponMesh->SetSimulatePhysics(true);
				WeaponMesh->SetEnableGravity(true);
				WeaponMesh->SetAllMassScale(1.f);
			}
		}

		UAnimMontage* SelectedDeathAnimation = GetHitAnimationDirectorComponent()->GetHitAnimation(DeathCauser, GetCapsuleComponent(), GetLastTakenDamage(), DeathAnimationsByDirection);
		GetAnimInstance()->ClearAllMontageCallbacks();
		GetAnimInstance()->PlayAnimationMontage(SelectedDeathAnimation);

		GetCapsuleComponent()->DestroyComponent();
		GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
		GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);
		GetMesh()->SetAllMassScale(EnemyMassScaleOnDeath);

		// clang-format off
		FTimerDelegate RagdollDelegate;
		RagdollDelegate.BindLambda(
			[this, PhysicalAnimationComponent]()
			{
				this->StartRagdollAnimationCallback.ExecuteIfBound();
			});

		FTimerHandle TimerHandle_RagdollDeath;
		FTimerHandle TimerHandle_Destroy;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_RagdollDeath, RagdollDelegate, RagdollInitialDelay, false);
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_Destroy, [&](){ Destroy(); }, DyingTime, false);
		// clang-format on

		OnDeath();

		return true;
	}
	return false;
}

ACharacter* AEnemyBase::GetCharacterSelf_Implementation()
{
	return Cast<ACharacter>(this);
}

USkeletalMeshComponent* AEnemyBase::GetCharacterMesh_Implementation() const
{
	return this->GetMesh();
}

AWeaponBase* AEnemyBase::GetCurrentWeapon_Implementation() const
{
	return this->CurrentWeapon;
}

EUnitState AEnemyBase::GetUnitState_Implementation() const
{
	return this->UnitState;
}

EUnitType AEnemyBase::GetUnitType_Implementation() const
{
	return this->UnitType;
}

bool AEnemyBase::GetIsDead_Implementation() const
{
	return this->bDead;
}

bool AEnemyBase::GetInvincibility_Implementation() const
{
	return this->bInvincibility;
}

void AEnemyBase::SetUnitState_Implementation(EUnitState NewUnitState)
{
	UnitState = NewUnitState;
	
	// little hack to make sure that unit got right movement settings while walking
	if (NewUnitState == EUnitState::Walking)
	{
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
	}
}

void AEnemyBase::SetInvincibility_Implementation(bool bSetInvisible)
{
	bInvincibility = bSetInvisible;
}

void AEnemyBase::SetCurrentWeapon_Implementation(AWeaponBase* Weapon)
{
	CurrentWeapon = Weapon;
}