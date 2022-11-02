// Fill out your copyright notice in the Description page of Project Settings.

#include "ShipRTSController.h"

AShipRTSController::AShipRTSController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
}

void AShipRTSController::BeginPlay()
{
	HUDptr = Cast<ARTSHUD>(GetHUD());
}

void AShipRTSController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("LeftMouseClick", IE_Pressed, this, &AShipRTSController::SelectionPressed);
	InputComponent->BindAction("LeftMouseClick", IE_Released, this, &AShipRTSController::SelectionReleased);

	InputComponent->BindAction("RightMouseClick", IE_Released, this, &AShipRTSController::MoveReleased);
}

void AShipRTSController::SelectionPressed()
{
	HUDptr->bStartSelecting = true;
	HUDptr->InitialSelectionPoint = HUDptr->GetMousePosition2D();
}

void AShipRTSController::SelectionReleased()
{
	HUDptr->bStartSelecting = false;
	selectedEntities = HUDptr->foundEntities;
}

void AShipRTSController::MoveReleased()
{
	FHitResult hit;
	GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, hit);
	for (int i = 0; i < selectedEntities.Num(); ++i) {
		AEntityPawn* entity = selectedEntities[i];

		FVector moveLocation = hit.Location + FVector(i / 2 * 100, i % 2 * 100, 0);
		entity->CommandMoveTo(moveLocation);
	}
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::White, TEXT("Move Released at ") + hit.Location.ToCompactString());
	DrawDebugSphere(GetWorld(), hit.Location, 50.0f, 16, FColor(255, 255, 255), false, 10.0f);
}