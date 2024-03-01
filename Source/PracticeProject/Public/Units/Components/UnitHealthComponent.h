// Copyright 2023 Vsevolod Khlebnikov. All Rights Reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"

#include "UnitHealthComponent.generated.h"

/**
 * Component that delegate logic of owner damage taking and health storing
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PRACTICEPROJECT_API UUnitHealthComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	/** Should call TakeDamage automatically OnTakeDamage event or manually */
	UPROPERTY(EditAnywhere, Category = "Behavior")
	bool bCallAutomatically;

	/** Inital health value of owner */
	UPROPERTY(EditAnywhere, Category = "Health")
	float InitialHealth;

	/** Current health value of owner */
	float Health;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	/** Wrap of engine TakeDamage function */
	UFUNCTION(BlueprintCallable, Category = "Unit Health Component")
	void TakeDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

public:
	// Sets default values for this component's properties
	UUnitHealthComponent();

	/** Get current health value of owner */
	UFUNCTION(BlueprintPure, Category = "Unit Health Component")
	float GetHealth() const { return Health; }

	/** Get initial health value of owner */
	UFUNCTION(BlueprintPure, Category = "Unit Health Component")
	float GetInitialHealth() const { return InitialHealth; }

	/** Set current health value of owner */
	UFUNCTION(BlueprintCallable, Category = "Unit Health Component")
	void SetHealth(float Value) { Health = Value; }

	/** Set initial health value of owner */
	UFUNCTION(BlueprintCallable, Category = "Unit Health Component")
	void SetInitialHealth(float Value) { InitialHealth = Value; }
};
