// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "../Components/Movement/ShipMovementComponent.h"
#include <Components/WidgetComponent.h>
#include <NiagaraComponent.h>

#include "EntityPawn.generated.h"

class ATurretPawnBase;
class USpringArmComponent;

UCLASS(Abstract)
class STARFLEET_API AEntityPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AEntityPawn();

	UFUNCTION(BlueprintCallable)
		void SetSelected(const bool selected);
	UFUNCTION(BlueprintCallable)
		bool IsCurrentlySelected() const;

	UFUNCTION(BlueprintCallable)
		bool IsOwnedByPlayer() const;

	UFUNCTION(BlueprintCallable)
		bool IsAlive() const;

	UFUNCTION(BlueprintCallable)
		void CauseDamage(const float damageVal);

	UFUNCTION(BlueprintCallable)
		void SetHealth(const float health);

	UFUNCTION(BlueprintCallable)
		float GetHealth() const;

	UFUNCTION(BlueprintCallable)
		void CommandMoveTo(const FVector& destination);
	UFUNCTION(BlueprintCallable)
		void CommandTurretsTarget(AActor* targetActor);

	virtual UPawnMovementComponent* GetMovementComponent() const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	void FillTurretSockets();
	void SpawnTurret(const FName& _TurretSocketName);
	void DestroyTurrets();

protected:
	bool isSelected;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Ship")
		bool ownedByPlayer;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		UShipMovementComponent* shipMoveComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* shipStaticMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		USpringArmComponent* SelectionSpringArm;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		UWidgetComponent* shipSelectionWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Niagara")
		TArray<UNiagaraComponent*> shipEngineParticleEffects;
	FVector moveTargetPosition;

	//engine particles
	UPROPERTY(EditDefaultsOnly, Category = "Particles")
		UNiagaraSystem* engineParticleEffectBP;
	UPROPERTY(EditDefaultsOnly, Category = "Particles")
		float engineSize;
	TArray<UNiagaraComponent*> engineParticleEffects;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Meta = (MakeEditWidget = true))
		TArray<FTransform> engineParticleEffectsPositions;

	//destruction geometry
	UPROPERTY(EditDefaultsOnly, Category = "Destruction")
		TSubclassOf<AActor> DestructionGeometryActorBP;
	//destruction particles
	UPROPERTY(EditDefaultsOnly, Category = "Particles")
		UNiagaraSystem* explosionEffectBP;
	UPROPERTY(EditDefaultsOnly, Category = "Particles")
		float explosionSize;

	//Turrets functionality
	UPROPERTY(Category = BlueprintSpawn, EditDefaultsOnly, BlueprintReadOnly)
		TSubclassOf<ATurretPawnBase> TurretBP;
	TArray<FName> TurretArray;
	UPROPERTY(Category = BlueprintSpawn, EditDefaultsOnly, BlueprintReadWrite)
		TArray<ATurretPawnBase*> SpawnedTurretArray;

	UPROPERTY(Category = "Stats", EditDefaultsOnly, BlueprintReadWrite)
		float fHealth;
};
