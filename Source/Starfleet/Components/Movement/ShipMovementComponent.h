// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <GameFramework/PawnMovementComponent.h>
#include "ShipMovementComponent.generated.h"

/**
 *
 */
UCLASS()
class STARFLEET_API UShipMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()

public:
	enum class EShipMovementState {
		Accelerating,
		Decelerating,
		Moving,
		Turning
	};
	UShipMovementComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	bool CheckState(EShipMovementState shipState);
	void SetShipMovementState(EShipMovementState shipState, bool value);

	virtual inline float GetMaxSpeed() const override;
	FVector GetCurrentSpeed() const;
	inline float GetForwardAcceleration() const;
	inline float GetMaxAngularSpeed() const;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
protected:
	uint8 currentMovementState;
	FVector CurrentVelocity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float fMaxSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float fForwardAcceleration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float fMaxAngularSpeed;

	FVector2D AccelerationPath;

	FQuat CurrentRotator;
};
