// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "../Components/Movement/ShipMovementComponent.h"
#include <Components/WidgetComponent.h>
#include <NiagaraComponent.h>

#include "EntityPawn.generated.h"

UCLASS()
class STARFLEET_API AEntityPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AEntityPawn();

	UFUNCTION()
		void SetSelected();
	UFUNCTION()
		void SetDeselected();

	void CommandMoveTo(const FVector& destination);

	virtual UPawnMovementComponent* GetMovementComponent() const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		UShipMovementComponent* shipMoveComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* shipStaticMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UWidgetComponent* shipSelectionWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Niagara")
		TArray<UNiagaraComponent*> shipEngineParticleEffects;
	FVector moveTargetPosition;

	UPROPERTY(EditAnywhere)
		UNiagaraSystem* engineParticleEffectBP;
	TArray<UNiagaraComponent*> engineParticleEffects;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (MakeEditWidget = true))
		TArray<FTransform> engineParticleEffectsPositions;
};
