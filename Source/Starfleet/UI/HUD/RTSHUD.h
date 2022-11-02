// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "../../Entities/EntityPawn.h"

/**
 *
 */
#include "RTSHUD.generated.h"

UCLASS()
class STARFLEET_API ARTSHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;

	FVector2D GetMousePosition2D();

public:
	bool bStartSelecting;

	FVector2D InitialSelectionPoint;
	FVector2D CurrentSelectionPosition;

	TArray<AEntityPawn*> foundEntities;
};
