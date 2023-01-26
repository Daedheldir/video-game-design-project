// Fill out your copyright notice in the Description page of Project Settings.

#include "ShipMovementComponent.h"

UShipMovementComponent::UShipMovementComponent(const FObjectInitializer& ObjectInitializer)
{
	currentMovementState |= 0b0;
}

bool UShipMovementComponent::CheckState(EShipMovementState shipState)
{
	return (currentMovementState & static_cast<uint8>(shipState)) != 0b0;
}
void UShipMovementComponent::SetShipMovementState(EShipMovementState shipState, bool value)
{
	if (value)
		this->currentMovementState |= static_cast<uint8>(shipState);
	else
		this->currentMovementState &= ~static_cast<uint8>(shipState);
}

inline float UShipMovementComponent::GetMaxSpeed() const {
	return fMaxSpeed;
}

inline float UShipMovementComponent::GetForwardAcceleration() const {
	return fForwardAcceleration;
}
inline float UShipMovementComponent::GetMaxAngularSpeed() const {
	return fMaxAngularSpeed;
}
void UShipMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!PawnOwner || !UpdatedComponent || ShouldSkipUpdate(DeltaTime)) {
		return;
	}

	FVector currentInputVector = ConsumeInputVector().GetClampedToMaxSize(1.0f);
	FVector desiredMovementThisFrame = currentInputVector * DeltaTime * GetMaxSpeed();
	FVector desiredForwardMovementThisFrame = FMath::Lerp(
		UpdatedComponent->GetForwardVector() * DeltaTime * GetMaxSpeed(),
		desiredMovementThisFrame,
		0.3);
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, TEXT("Processing command \"Move to\" of object ") + UpdatedComponent->GetName() + FString::Printf(TEXT(", Vector is %f"), GetMaxSpeed()));

	double angleDifference = (currentInputVector.HeadingAngle()) - (UpdatedComponent->GetForwardVector().HeadingAngle());
	if (angleDifference > PI) {
		angleDifference -= 2 * PI;
	}
	else if (angleDifference < -PI) {
		angleDifference += 2 * PI;
	}

	if (!desiredMovementThisFrame.IsNearlyZero(10.0)) {
		double desiredAngleThisFrame = UpdatedComponent->GetForwardVector().HeadingAngle();

		if (std::abs(angleDifference) >= GetMaxAngularSpeed() * DeltaTime)
			desiredAngleThisFrame += (angleDifference < 0 ? -GetMaxAngularSpeed() : GetMaxAngularSpeed()) * DeltaTime;
		else
			desiredAngleThisFrame = currentInputVector.HeadingAngle();

		FQuat rotationQuat = FQuat(UpdatedComponent->GetUpVector(), desiredAngleThisFrame);

		FHitResult Hit;

		if (!SafeMoveUpdatedComponent(desiredForwardMovementThisFrame, rotationQuat, true, Hit)) {
			check(GEngine); GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, TEXT("MoveUpdatedComponent returned false"));
		}

		// If we bumped into something, try to slide along it
		if (Hit.IsValidBlockingHit())
		{
			SlideAlongSurface(desiredMovementThisFrame, 1.f - Hit.Time, Hit.Normal, Hit);
		}
	}
	UpdateComponentVelocity();
}