// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TurretInterface.h"
#include <NiagaraComponent.h>

#include "TurretPawnBase.generated.h"

//forward declarations
class AMunitionsBase;

UCLASS(Abstract, meta = (BlueprintSpawnableComponent))
class STARFLEET_API ATurretPawnBase : public APawn, public ITurretInterface
{
	GENERATED_BODY()

public:
	ATurretPawnBase(const FObjectInitializer& ObjectInitializer);
	UPROPERTY(Category = Mesh, EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* TurretBaseMesh;
	UPROPERTY(Category = Mesh, EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* TurretHullMesh;
	UPROPERTY(Category = Mesh, EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* TurretBarrelMesh;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetCurrentTurretTarget(AActor* newTarget);
	void TurretLookAt();

	// Spawns Munitions
	UPROPERTY(Category = BlueprintSpawn, EditDefaultsOnly, BlueprintReadOnly)
		UNiagaraSystem* MunitionsParticleEffectBP;
	UPROPERTY(Category = BlueprintSpawn, EditDefaultsOnly, BlueprintReadOnly)
		TSubclassOf<AMunitionsBase> MunitionsBP;
	TArray<FName> MuzzleArray;

	void GetMuzzleSockets();
	void SpawnMunitions(FName _MuzzleSocketName);

	//Ownership
	UFUNCTION()
		bool IsOwnedByPlayer() const;
	UFUNCTION()
		void SetOwnedByPlayer(bool owned);

	//ITurretInterface
	virtual void PrimaryFireControl() override;

	FRotator TurretLocalRot;

	UPROPERTY(Category = "Turret | Chasis", BlueprintReadWrite, EditDefaultsOnly)
		float TurretYawSpeed;
	UPROPERTY(Category = "Turret | Chasis", BlueprintReadWrite, EditDefaultsOnly)
		float TurretMaxYaw;
	UPROPERTY(Category = "Turret | Chasis", BlueprintReadWrite, EditDefaultsOnly)
		float TurretMinYaw;
	UPROPERTY(Category = "Turret | Barrel", BlueprintReadWrite, EditDefaultsOnly)
		float TurretPitchSpeed;
	UPROPERTY(Category = "Turret | Barrel", BlueprintReadWrite, EditDefaultsOnly)
		float TurretMaxPitch;
	UPROPERTY(Category = "Turret | Barrel", BlueprintReadWrite, EditDefaultsOnly)
		float TurretMinPitch;
	UPROPERTY(Category = "Turret | Control", BlueprintReadOnly, EditDefaultsOnly)
		bool IsOccupied;
	UPROPERTY(Category = "Turret | Control", BlueprintReadOnly, VisibleAnywhere)
		bool IsTargeting;

	UPROPERTY(Category = "Turret | Control", BlueprintReadWrite, EditDefaultsOnly)
		float refireDelay;
	UPROPERTY(Category = "Turret | Control", BlueprintReadWrite, EditDefaultsOnly)
		float MaxRange;
	UPROPERTY(Category = "Turret | Control", BlueprintReadWrite, EditDefaultsOnly)
		float AccuracyDeviation;

	float currentTime;
protected:
	UPROPERTY(Category = DEBUG, BlueprintReadWrite, EditDefaultsOnly)
		bool DEBUG_MODE;

private:
	UPROPERTY(Category = "Turret | Control", VisibleAnywhere)
		bool ownedByPlayer;
	AActor* CurrentTurretTarget;
	bool IsFiring;

	float CurrentChasisRot;
	float CurrentBarrelRot;
	float CurrentYawMin;
	float CurrentYawMax;

	FRotator CurrentTurretRot;
	FRotator TurretYawRot;
	FRotator TurretPitchRot;
};
