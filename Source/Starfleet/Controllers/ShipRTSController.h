// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include <DrawDebugHelpers.h>
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "../UI/HUD/RTSHUD.h"
#include "../Entities/EntityPawn.h"
#include "../Managers/EntityManagerActor.h"
/**
 *
 */
#include "ShipRTSController.generated.h"

UCLASS()
class STARFLEET_API AShipRTSController : public APlayerController
{
	GENERATED_BODY()

public:
	AShipRTSController();
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

protected:
	//Entity spawning
	void SpawnEntity();

	// Entity commanding
	void SelectionPressed();
	void SelectionReleased();
	void MoveReleased();

	//Camera movement
	void CameraMoveForward(float axisVal);
	void CameraMoveSide(float axisVal);
	void CameraMoveHeight(float axisVal);
	FVector GetCameraMovementNewLocation(const FVector& newLocationOffset) const;

public:
	ARTSHUD* HUDptr;

private:
	//entity spawning
	AEntityManagerActor* entityManager;

	//entity selection
	TArray<AEntityPawn*> selectedEntities;

	//camera movement
	UPROPERTY(EditAnywhere)
		FVector cameraMovementScale;
	UPROPERTY(EditAnywhere)
		FVector cameraMovementAccScale;
	FVector currentCameraMovementVelocity;
};
