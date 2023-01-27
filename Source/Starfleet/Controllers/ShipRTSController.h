// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include <DrawDebugHelpers.h>
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "../Managers/EntityManagerActor.h"
#include "../Entities/EEntityTypes.h"
/**
 *
 */
#include "ShipRTSController.generated.h"

 //FORWARD DECLARATIONS
class ARTSHUD;
class AInputStateInfo;
class AEntityPawn;

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
	void SpawnEntity(EntityTypes enitityType);

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
	AInputStateInfo* InputState;
private:
	//entity spawning
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<AEntityManagerActor> entityManager_BP_Class;
	AEntityManagerActor* entityManager;
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<AEntityManagerActor> enemyEntityManager_BP_Class;
	AEntityManagerActor* enemyEntityManager;

	//entity selection
	TArray<AEntityPawn*> selectedEntities;

	//camera movement
	UPROPERTY(EditAnywhere)
		FVector cameraMovementScale;
	UPROPERTY(EditAnywhere)
		FVector cameraMovementAccScale;
	FVector currentCameraMovementVelocity;
};
