// Fill out your copyright notice in the Description page of Project Settings.

#include "EntityPawn.h"

// Sets default values
AEntityPawn::AEntityPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	shipStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ship Mesh"));
	shipStaticMesh->SetupAttachment(RootComponent);

	shipMoveComponent = CreateDefaultSubobject<UShipMovementComponent>(TEXT("Ship Movement Component"));
	GetMovementComponent()->SetPlaneConstraintNormal(FVector(0, 0, 1));
	GetMovementComponent()->SetPlaneConstraintEnabled(true);
	GetMovementComponent()->bSnapToPlaneAtStart = true;
	GetMovementComponent()->SetUpdatedComponent(this->GetRootComponent());

	shipSelectionWidget = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ship Selection Widget"));
	shipSelectionWidget->SetupAttachment(shipStaticMesh);
	shipSelectionWidget->SetVisibility(false);
}

void AEntityPawn::SetSelected()
{
	shipSelectionWidget->SetVisibility(true);
}

void AEntityPawn::SetDeselected()
{
	shipSelectionWidget->SetVisibility(false);
}

void AEntityPawn::CommandMoveTo(const FVector& destination) {
	moveTargetPosition = destination;
}

UPawnMovementComponent* AEntityPawn::GetMovementComponent() const {
	return shipMoveComponent;
}

// Called when the game starts or when spawned
void AEntityPawn::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AEntityPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector currentLocation = GetActorLocation();

	if (FVector::Dist2D(currentLocation, moveTargetPosition) > 10.0f) {
		FVector directionVec = { moveTargetPosition.X - currentLocation.X, moveTargetPosition.Y - currentLocation.Y, 0 };
		directionVec = directionVec.GetClampedToMaxSize(1.0f);

		if (GetMovementComponent() != nullptr) {
			GetMovementComponent()->AddInputVector(directionVec, true);
			//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, TEXT("Command \"Move to\" given with vector ") + directionVec.ToCompactString());
		}
		else {
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("GetMovementComponent() returned nullptr!"));
		}
	}
}

// Called to bind functionality to input
void AEntityPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}