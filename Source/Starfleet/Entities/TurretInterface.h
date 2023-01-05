// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TurretInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UTurretInterface : public UInterface {
	GENERATED_UINTERFACE_BODY()
};

/**
 *
 */
class ITurretInterface {
	GENERATED_IINTERFACE_BODY()

public:
	virtual void PrimaryFireControl() = 0;
};
