// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

#include "MunitionsBase.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;
class UPointLightComponent;

UCLASS(Abstract)
class STARFLEET_API AMunitionsBase : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AMunitionsBase();

	UFUNCTION()
		bool IsOwnedByPlayer() const;
	UFUNCTION()
		void SetOwnedByPlayer(bool owned);

	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
			UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(VisibleDefaultsOnly, Category = "Collision")
		USphereComponent* CollisionComponent;
	UPROPERTY(VisibleAnywhere, Category = "Collision")
		bool ownedByPlayer;
	UPROPERTY(VisibleAnywhere, Category = "Movement")
		UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Light")
		UPointLightComponent* pointLight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Particles")
		UNiagaraComponent* projectileParticles;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Particles")
		UNiagaraSystem* explosionEffectBP;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Particles")
		float explosionSize;
};
