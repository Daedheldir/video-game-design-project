// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include <DrawDebugHelpers.h>
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "../UI/HUD/RTSHUD.h"
#include "../Entities/EntityPawn.h"

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
	void SelectionPressed();
	void SelectionReleased();
	void MoveReleased();

public:
	ARTSHUD* HUDptr;

private:
	TArray<AEntityPawn*> selectedEntities;
};
