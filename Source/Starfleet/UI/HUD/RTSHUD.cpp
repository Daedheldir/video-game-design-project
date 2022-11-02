// Fill out your copyright notice in the Description page of Project Settings.

#include "RTSHUD.h"

void ARTSHUD::DrawHUD()
{
	if (bStartSelecting == true) {
		CurrentSelectionPosition = GetMousePosition2D();
		DrawRect(FLinearColor(0, 0, 1, 0.15f),
			InitialSelectionPoint.X,
			InitialSelectionPoint.Y,
			CurrentSelectionPosition.X - InitialSelectionPoint.X,
			CurrentSelectionPosition.Y - InitialSelectionPoint.Y);

		if (foundEntities.Num() > 0) {
			for (int i = 0; i < foundEntities.Num(); ++i) {
				foundEntities[i]->SetDeselected();
			}
		}
		foundEntities.Empty();
		GetActorsInSelectionRectangle<AEntityPawn>(InitialSelectionPoint, CurrentSelectionPosition, foundEntities, false, false);

		if (foundEntities.Num() > 0) {
			for (int i = 0; i < foundEntities.Num(); ++i) {
				foundEntities[i]->SetSelected();
			}
		}
	}
}

FVector2D ARTSHUD::GetMousePosition2D()
{
	float posX, posY;
	GetOwningPlayerController()->GetMousePosition(posX, posY);
	return FVector2D(posX, posY);
}