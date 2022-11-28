// Fill out your copyright notice in the Description page of Project Settings.

#include "RTSHUD.h"

ARTSHUD::ARTSHUD() {
	//static ConstructorHelpers::FClassFinder<UUserWidget> HealthBarObj(TEXT("/Game/FirstPerson/UI/Health_UI"));
	//HUDWidgetClass = HealthBarObj.Class;
}

void ARTSHUD::BeginPlay() {
	Super::BeginPlay();

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, TEXT("Creating command points menu"));
	if (commandPointsMenu)
	{
		commandPointsMenu->AddToViewport();
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, TEXT("Command points menu added to viewport"));
	}
}

void ARTSHUD::DrawHUD()
{
	if (bStartSelecting == true) {
		CurrentSelectionPosition = GetMousePosition2D();
		DrawRect(FLinearColor(0, 0, 1, 0.15f),
			InitialSelectionPoint.X,
			InitialSelectionPoint.Y,
			CurrentSelectionPosition.X - InitialSelectionPoint.X,
			CurrentSelectionPosition.Y - InitialSelectionPoint.Y);

		TArray<AEntityPawn*> previouslyFoundEntities = foundEntities;

		//if (foundEntities.Num() > 0) {
		//	for (auto & entity : foundEntities) {
		//		entity->SetDeselected();
		//	}
		//}
		foundEntities.Empty();
		GetActorsInSelectionRectangle<AEntityPawn>(InitialSelectionPoint, CurrentSelectionPosition, foundEntities, false, false);

		//set found entitites which weren't selected previously to selected and remove them from previously found entities
		if (foundEntities.Num() > 0) {
			for (auto& entity : foundEntities) {
				if (previouslyFoundEntities.Find(entity) >= 0) {
					previouslyFoundEntities.Remove(entity);
				}
				else {
					entity->SetSelected();
				}
			}
		}
		//deselect entities which were previously found and are no longer selected
		if (previouslyFoundEntities.Num() > 0) {
			for (auto& entity : previouslyFoundEntities) {
				entity->SetDeselected();
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