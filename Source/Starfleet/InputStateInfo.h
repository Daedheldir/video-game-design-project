// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "Entities/EEntityTypes.h"

#include "InputStateInfo.generated.h"

/**
 *
 */

UCLASS()
class STARFLEET_API AInputStateInfo : public AInfo
{
	GENERATED_BODY()

public:

	AInputStateInfo();

	enum class InputState {
		NONE,
		SELECTING,
		PLACING_SHIP
	};

	InputState GetCurrentInputState() const;
	void SetCurrentInputState(InputState newState);
	void SetLastSelectedSpawnShipButton(EntityTypes entityType);
	EntityTypes GetLastSelectedSpawnShipButton() const;
private:
	InputState inputState;
	EntityTypes lastSelectedEntityType;
};
