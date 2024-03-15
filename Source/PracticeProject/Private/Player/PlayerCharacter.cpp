// Copyright 2023 Vsevolod Khlebnikov. All Rights Reserved.

#include "Player/PlayerCharacter.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "PracticeProject/PracticeProjectCustomLogs.h"
#include "TimerManager.h"

DEFINE_LOG_CATEGORY(Player_LOG);

APlayerCharacter::APlayerCharacter()
{
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	GetSpringArmComponent()->SetupAttachment(RootComponent);
	GetSpringArmComponent()->bUsePawnControlRotation = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	GetCameraComponent()->SetupAttachment(GetSpringArmComponent());
	GetCameraComponent()->bUsePawnControlRotation = false;

	HitAnimationDirectorComponent = CreateDefaultSubobject<UHitAnimationDirectorComponent>(TEXT("HitAnimationDirectorComponent"));
	HealthComponent = CreateDefaultSubobject<UUnitHealthComponent>(TEXT("HealthComponent"));

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 360.0f, 360.0f);
	GetCharacterMovement()->JumpZVelocity = 450.f;
	GetCharacterMovement()->MaxWalkSpeed = 350.f;
	GetCharacterMovement()->AirControl = 0.2f;

	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	UnitState = EUnitState::Walking;
	UnitType = EUnitType::Player;

	MannaCostOfHeavyAttack = 300;
	StaminaCostOfCurrentCombo = 0;
	StaminaCostOfRoll = 55;

	HeavyAttackPreStrafeTime = 0.2f;

	StaminaCapacity = 100;
	MannaCapacity = 800;

	HighestDistanceToFocusTarget = 700.0f;
	CharacterSpeedInFocusRate = 0.58f;
	FocusRollOffset = 300.0f;
	FocusInterpSpeed = 5.0f;

	VolumeFadeTimeAfterDeath = 5.0f;
	HealthTresholdForDeath = 0.1f;
	PlayerMassScaleOnDeath = 5.0f;
	RollShakeInitialDelay = 0.3f;
	RagdollInitialDelay = 0.3f;
	DeathSoundVolume = 0.4;

	CameraShakeInnerRadius = 100.f;
	CameraShakeOuterRadius = 500.f;
	CameraShakeFalloff = 1.0f;

	bInvincibility = false;
	bInFocus = false;
	bDead = false;

	bHasComboAttacks = false;
	bMustPerformNextAttack = false;
	bAttackWindowBegun = false;
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (Cast<UAdvancedAnimInstance>(GetMesh()->GetAnimInstance()))
	{
		AnimInstance = Cast<UAdvancedAnimInstance>(GetMesh()->GetAnimInstance());
		GetAnimInstance()->SetOwner(this);
		Execute_SetUnitState(this, EUnitState::Walking);
	}
	else
	{
		UE_LOG(Player_LOG, Warning, TEXT("Wrong AnimInstance class"));
	}

	GetWorld()->GetFirstPlayerController()->PlayerCameraManager->ViewPitchMax = 0.0f;

	Stamina = StaminaCapacity;
	Manna = MannaCapacity;
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward/Backwards", this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight/Left", this, &APlayerCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APlayerCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APlayerCharacter::AddControllerPitchInput);

	PlayerInputComponent->BindAction("SetFocus", IE_Pressed, this, &APlayerCharacter::SetFocus);
	PlayerInputComponent->BindAction("SwitchFocus", IE_Pressed, this, &APlayerCharacter::SwitchFocus);
}

void APlayerCharacter::AddControllerYawInput(float Val)
{
	if (!bInFocus)
	{
		Super::AddControllerYawInput(Val);
	}
}

void APlayerCharacter::AddControllerPitchInput(float Val)
{
	Super::AddControllerPitchInput(Val);
}

float APlayerCharacter::GetCharacterWalkingSpeed() const
{
	return GetCharacterMovement()->MaxWalkSpeed;
}

void APlayerCharacter::AddManna(int Amount)
{
	Manna = Manna + Amount > MannaCapacity ? MannaCapacity : Manna + Amount;
}

void APlayerCharacter::SetCharacterWalkingSpeed(float Speed)
{
	GetCharacterMovement()->MaxWalkSpeed = Speed;
}

void APlayerCharacter::HeavyAttack(TSubclassOf<UCameraShakeBase> CameraShakeClassToSet)
{
	SetCameraShakeClass(CameraShakeClassToSet);
	if (UnitState == EUnitState::Walking)
	{
		if (GetManna() > 0)
		{
			StrafeByInputDirectionCallback.ExecuteIfBound();

			UAnimMontage* HeavyAttackMontage = CurrentWeapon->GetMovesetDirectorComponent()->GetHeavyAttack();

			// clang-format off
			GetAnimInstance()->ClearAllMontageCallbacks();
			GetAnimInstance()->OnMontageNotifyBeginCallback.AddUniqueDynamic(this, &APlayerCharacter::HeavyAttackOnNotifyBegin); 
			GetAnimInstance()->OnMontageBlendOutCallback.AddUniqueDynamic(this, &APlayerCharacter::HeavyAttackOnBlendOut);
			GetAnimInstance()->PlayAnimationMontage(HeavyAttackMontage);
			// clang-format on

			Manna = GetManna() - MannaCostOfHeavyAttack;
			Execute_SetUnitState(this, EUnitState::HeavyAttack);
		}
	}
}

void APlayerCharacter::HeavyAttackOnNotifyBegin()
{
	StrafeByCameraDirectionCallback.ExecuteIfBound();

	// clang-format off
	FTimerHandle TimerHandle_LockRotation;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_LockRotation, [&](){ bUseControllerRotationYaw = true; }, HeavyAttackPreStrafeTime, false);
	StartCameraShake();
	// clang-format on
}

void APlayerCharacter::HeavyAttackOnBlendOut()
{
	bUseControllerRotationYaw = false;
	Execute_SetUnitState(this, EUnitState::Walking);
}

void APlayerCharacter::RandomComboAttack(TSubclassOf<UCameraShakeBase> CameraShakeClassToSet)
{
	SetCameraShakeClass(CameraShakeClassToSet);
	if (UnitState == EUnitState::Walking)
	{
		ComboAttack();
	}
	else
	{
		if (bAttackWindowBegun == true)
		{
			bMustPerformNextAttack = true; // player hit combo attack key in right moment and continue attack
		}
	}
}

void APlayerCharacter::ComboAttack()
{
	if (StaminaCostOfCurrentCombo > GetStamina() || UnitState == EUnitState::Stan || UnitState == EUnitState::Death) // different cases at we should interrupt combo chain
	{
		StartToFinishComboAttack();
	}
	else
	{
		UAnimMontage* ComboAttackMontage = CurrentWeapon->GetMovesetDirectorComponent()->GetComboAttack(CurrentCombo, bHasComboAttacks, StaminaCostOfCurrentCombo);
		if (bHasComboAttacks == true && UnitState == EUnitState::Walking && ComboAttackMontage)
		{
			bMustPerformNextAttack = false;
			TakeStamina(StaminaCostOfCurrentCombo);

			StrafeByInputDirectionCallback.ExecuteIfBound();

			// clang-format off
			GetAnimInstance()->ClearAllMontageCallbacks();
			GetAnimInstance()->OnMontageInterruptedCallback.AddUniqueDynamic(this, &APlayerCharacter::StartToFinishComboAttack);
			GetAnimInstance()->OnMontageBlendOutCallback.AddUniqueDynamic(this, &APlayerCharacter::ComboAttackOnBlendOut);
			GetAnimInstance()->OnMontageNotifyBeginCallback.AddUniqueDynamic(this, &APlayerCharacter::ComboAttackOnBeginNotify);
			GetAnimInstance()->OnMontageNotifyEndCallback.AddUniqueDynamic(this, &APlayerCharacter::ComboAttackOnEndNotify);
			GetAnimInstance()->PlayAnimationMontage(ComboAttackMontage);
			// clang-format on

			Execute_SetUnitState(this, EUnitState::ComboAttack);
		}
		else // combo chain completed and has no attacks more or just interrupted
		{
			StartToFinishComboAttack();
		}
	}
}

void APlayerCharacter::StartToFinishComboAttack()
{
	CurrentWeapon->GetMovesetDirectorComponent()->ResetCurrentComboAttackIndex();
	bMustPerformNextAttack = false;
	bAttackWindowBegun = false;
	Execute_SetUnitState(this, EUnitState::Walking);
}

void APlayerCharacter::ComboAttackOnBeginNotify()
{
	bAttackWindowBegun = true;
	StartCameraShake();
}

void APlayerCharacter::ComboAttackOnEndNotify()
{
	bAttackWindowBegun = false;
}

void APlayerCharacter::ComboAttackOnBlendOut()
{
	if (bMustPerformNextAttack == true)
	{
		ComboAttack();
	}
	else
	{
		StartToFinishComboAttack();
	}
}

FString APlayerCharacter::GetRecoveryType(bool bParryKeyPressed) const
{
	if (UnitState == EUnitState::Walking)
	{
		return bParryKeyPressed == true ? "Parry" : "StateRecovery";
	}
	else
	{
		return "Fail";
	}
}

void APlayerCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		const FRotator ControllerRotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.0f, ControllerRotation.Yaw, 0.0f);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

void APlayerCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		const FRotator ControllerRotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.0f, ControllerRotation.Yaw, 0.0f);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void APlayerCharacter::RightDodge(UAnimMontage* DodgeAnimation)
{
	if (UnitState == EUnitState::Walking)
	{
		StrafeByCameraDirectionCallback.ExecuteIfBound();
		GetAnimInstance()->ClearAllMontageCallbacks();
		GetAnimInstance()->PlayAnimationMontage(DodgeAnimation);
		Execute_SetUnitState(this, EUnitState::Dodge);
	}
}

void APlayerCharacter::LeftDodge(UAnimMontage* DodgeAnimation)
{
	if (UnitState == EUnitState::Walking)
	{
		StrafeByCameraDirectionCallback.ExecuteIfBound();
		GetAnimInstance()->ClearAllMontageCallbacks();
		GetAnimInstance()->PlayAnimationMontage(DodgeAnimation);
		Execute_SetUnitState(this, EUnitState::Dodge);
	}
}

void APlayerCharacter::Roll(UAnimMontage* RollAnimation, TSubclassOf<UCameraShakeBase> CameraShakeClassToSet)
{
	SetCameraShakeClass(CameraShakeClassToSet);
	if (UnitState == EUnitState::Walking)
	{
		if (TakeStamina(StaminaCostOfRoll))
		{
			StrafeByInputDirectionCallback.ExecuteIfBound();

			// clang-format off
			GetAnimInstance()->ClearAllMontageCallbacks();
			GetAnimInstance()->OnMontageNotifyBeginCallback.AddUniqueDynamic(this, &APlayerCharacter::RollOnBeginNotify);
			GetAnimInstance()->OnMontageNotifyEndCallback.AddUniqueDynamic(this, &APlayerCharacter::RollOnEndNotify);
			GetAnimInstance()->OnMontageBlendOutCallback.AddUniqueDynamic(this, &APlayerCharacter::RollOnBlendOut);
			GetAnimInstance()->PlayAnimationMontage(RollAnimation);
			// clang-format on

			Execute_SetUnitState(this, EUnitState::Roll);
		}
	}
}

void APlayerCharacter::RollOnBeginNotify()
{
	bInvincibility = true;
	FTimerHandle TimerHandle_RollShakeFX;
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle_RollShakeFX,
		[&]()
		{
			StartCameraShake();
		},
		RollShakeInitialDelay, false);
}

void APlayerCharacter::RollOnEndNotify()
{
	bInvincibility = false;
	Execute_SetUnitState(this, EUnitState::Walking);
}

void APlayerCharacter::RollOnBlendOut()
{
	bInvincibility = false;
}

void APlayerCharacter::StartCameraShake()
{
	const APlayerCameraManager* CameraManager = GetWorld()->GetFirstPlayerController()->PlayerCameraManager;
	CameraManager->PlayWorldCameraShake(GetWorld(), GetCameraShakeClass(), GetActorLocation(), CameraShakeInnerRadius, CameraShakeOuterRadius, CameraShakeFalloff, false);
}

bool APlayerCharacter::TakeStamina(int Difference)
{
	if (GetStamina() >= Difference && Stamina != 0)
	{
		Stamina = GetStamina() - Difference;
		return true;
	}
	else
	{
		return false;
	}
}

void APlayerCharacter::RegainStamina()
{
	if (UnitState == EUnitState::Walking && GetStamina() < StaminaCapacity)
	{
		Stamina = GetStamina() + 1;
	}
}

bool APlayerCharacter::OnTakeDamage(float Damage, AActor* DamageCauser, TSubclassOf<UCameraShakeBase> CameraShakeClassToSet)
{
	if (UnitState != EUnitState::Stan)
	{
		SetCameraShakeClass(CameraShakeClassToSet);
		UAnimMontage* HitMontage = GetHitAnimationDirectorComponent()->GetHitAnimation(DamageCauser, GetCapsuleComponent(), Damage, FAnimationsByDirection());

		if (IsValid(HitMontage))
		{
			CurrentHitAnimation = HitMontage;
			StopAnimMontage(GetCurrentMontage());

			// clang-format off
			GetAnimInstance()->ClearAllMontageCallbacks();
			GetAnimInstance()->OnMontageBlendOutCallback.AddUniqueDynamic(this, &APlayerCharacter::OnTakeDamageBlendOut);
			GetAnimInstance()->PlayAnimationMontage(HitMontage);
			// clang-format on

			StartCameraShake();

			bInvincibility = true;
			Execute_SetUnitState(this, EUnitState::Stan);

			return true;
		}
		else
		{
			UE_LOG(Player_LOG, Error, TEXT("HitMontage is not valid"));
		}
	}
	return false;
}

void APlayerCharacter::OnTakeDamageBlendOut()
{
	bUseControllerRotationYaw = false;
	bInvincibility = false;
	CurrentHitAnimation = nullptr;
	Execute_SetUnitState(this, EUnitState::Walking);
}

void APlayerCharacter::Death(FPlayerDeathData DeathData)
{
	if (HealthComponent->GetHealth() <= HealthTresholdForDeath)
	{
		if (UnitState != EUnitState::Death)
		{
			bDead = true;
			Execute_SetUnitState(this, EUnitState::Death);
			UE_LOG(Player_LOG, Display, TEXT("Player is dead"));

			TArray<AActor*> AttachedItems;
			this->GetAttachedActors(AttachedItems);

			//detach weapons from player
			for (int i = 0; i < AttachedItems.Num(); i++)
			{
				AttachedItems[i]->DetachFromActor(FDetachmentTransformRules(FAttachmentTransformRules(EAttachmentRule::KeepWorld, false), false));
				if (AttachedItems[i]->GetClass()->ImplementsInterface(UWeaponInterface::StaticClass()))
				{
					const IWeaponInterface* Interface = Cast<IWeaponInterface>(AttachedItems[i]);
					UStaticMeshComponent* WeaponMesh = Interface->Execute_GetWeaponMeshComponent(AttachedItems[i]);
					if (WeaponMesh != nullptr)
					{
						WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
						WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
						WeaponMesh->SetSimulatePhysics(true);
						WeaponMesh->SetEnableGravity(true);
						WeaponMesh->SetAllMassScale(1.f);
					}
				}
			}

			GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			this->DisableInput(UGameplayStatics::GetPlayerController(GetWorld(), 0));

			UGameplayStatics::SpawnSoundAttached(DeathData.DeathSound, GetMesh(), FName(TEXT("pelvis")), GetMesh()->GetRelativeLocation(), EAttachLocation::KeepRelativeOffset, false, DeathSoundVolume);

			UDeathScreenWidget* DeathScreen = CreateWidget<UDeathScreenWidget>(GetWorld(), DeathData.DeathScreenClass);
			DeathScreen->AddToViewport();

			GetMesh()->SetCollisionResponseToAllChannels(ECR_Ignore);
			GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
			GetMesh()->SetMassScale("pelvis", PlayerMassScaleOnDeath);

			if (GetCurrentHitAnimation() == nullptr)
			{
				GetAnimInstance()->ClearAllMontageCallbacks();
				GetAnimInstance()->PlayAnimationMontage(DeathData.DeathAnimation);
			}

			FTimerDelegate RagdollDelegate;
			RagdollDelegate.BindLambda(
				[this, DeathData]()
				{
					this->StartRagdollAnimationCallback.ExecuteIfBound();
				});

			FTimerHandle TimerHandle_RagdollDeath;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle_RagdollDeath, RagdollDelegate, RagdollInitialDelay, false);

			UGameplayStatics::SetSoundMixClassOverride(GetWorld(), DeathData.DeathThemeMix, DeathData.DeathThemeClass, 0.0f, 1.0f, VolumeFadeTimeAfterDeath, true);
			const float MenuOpeningTime = VolumeFadeTimeAfterDeath;
			FTimerHandle TimerHandle_OpenMenu;
			GetWorld()->GetTimerManager().SetTimer(
				TimerHandle_OpenMenu, [&]()
				{ UGameplayStatics::OpenLevel(GetWorld(), "Menu"); },
				MenuOpeningTime, false);
		}
		else
		{
			UE_LOG(Player_LOG, Warning, TEXT("Player is already dead"));
		}
	}
}

void APlayerCharacter::SetFocus()
{
	if (GetInFocus())
	{
		ClearFocus();
	}
	else
	{
		if (FindFocusTarget())
		{
			bInFocus = true;

			GetCharacterMovement()->bUseControllerDesiredRotation = true;
			GetCharacterMovement()->bOrientRotationToMovement = false;
			GetCharacterMovement()->MaxWalkSpeed = (GetCharacterWalkingSpeed() * CharacterSpeedInFocusRate);

			constexpr float DeltaTime = 0.01f; // Delta time between focus calls
			GetWorld()->GetTimerManager().SetTimer(TimerHandle_Focus, this, &APlayerCharacter::UpdateFocus, DeltaTime, true);
		}
	}
}

void APlayerCharacter::SwitchFocus()
{
	if (GetInFocus())
	{
		FindFocusTarget();
	}
}

void APlayerCharacter::ClearFocus()
{
	bInFocus = false;
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Focus);
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
}

bool APlayerCharacter::FindFocusTarget()
{
	TArray<AActor*> Enemies;
	UGameplayStatics::GetAllActorsWithTag(this, FName(TEXT("Enemy")), Enemies);

	float LowestTargetDistance = HighestDistanceToFocusTarget;
	for (AActor* Enemy : Enemies)
	{
		float DistanceToCurrentEnemy = FVector::Distance(Enemy->AActor::GetActorLocation(), AActor::GetActorLocation());
		if (DistanceToCurrentEnemy < LowestTargetDistance)
		{
			LowestTargetDistance = DistanceToCurrentEnemy;
			FocusTarget = Enemy;
		}
	}
	if (FocusTarget == nullptr)
	{
		ClearFocus();
		return false;
	}
	return true;
}

void APlayerCharacter::UpdateFocus()
{
	if (IsValid(FocusTarget))
	{
		if (FocusTarget->GetClass()->ImplementsInterface(UCharacterInterface::StaticClass()))
		{
			const ICharacterInterface* Interface = Cast<ICharacterInterface>(FocusTarget);
			if (Interface->Execute_GetIsDead(FocusTarget))
			{
				ClearFocus();
			}
			else
			{
				const FVector CameraLocation = FVector(FocusTarget->GetActorLocation().X, FocusTarget->GetActorLocation().Y, FocusTarget->GetActorLocation().Z + FocusRollOffset);
				const FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(AActor::GetActorLocation(), CameraLocation);

				const FRotator PlayerRotation = APawn::GetController()->AActor::GetActorRotation();
				const FRotator InterpolatedRotation = UKismetMathLibrary::RInterpTo(PlayerRotation, TargetRotation, UGameplayStatics::GetWorldDeltaSeconds(this), FocusInterpSpeed);

				const FRotator FixedTargetRotation = UKismetMathLibrary::MakeRotator(InterpolatedRotation.Roll, PlayerRotation.Pitch, InterpolatedRotation.Yaw);

				APawn::GetController()->AController::SetControlRotation(FixedTargetRotation);
			}
		}
	}
	else
	{
		UE_LOG(Player_LOG, Warning, TEXT("FocusTarget is not valid"));
	}
}

ACharacter* APlayerCharacter::GetCharacterSelf_Implementation()
{
	return Cast<ACharacter>(this);
}

USkeletalMeshComponent* APlayerCharacter::GetCharacterMesh_Implementation() const
{
	return this->GetMesh();
}

AWeaponBase* APlayerCharacter::GetCurrentWeapon_Implementation() const
{
	return this->CurrentWeapon;
}

EUnitState APlayerCharacter::GetUnitState_Implementation() const
{
	return this->UnitState;
}

EUnitType APlayerCharacter::GetUnitType_Implementation() const
{
	return this->UnitType;
}

bool APlayerCharacter::GetIsDead_Implementation() const
{
	return this->bDead;
}

bool APlayerCharacter::GetInvincibility_Implementation() const
{
	return this->bInvincibility;
}

void APlayerCharacter::SetUnitState_Implementation(EUnitState NewUnitState)
{
	UnitState = NewUnitState;
}

void APlayerCharacter::SetInvincibility_Implementation(bool bSetInvisible)
{
	bInvincibility = bSetInvisible;
}

void APlayerCharacter::SetCurrentWeapon_Implementation(AWeaponBase* Weapon)
{
	CurrentWeapon = Weapon;
}