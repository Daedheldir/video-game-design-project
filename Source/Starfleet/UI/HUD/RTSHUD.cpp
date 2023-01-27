// Fill out your copyright notice in the Description page of Project Settings.

#include "RTSHUD.h"
#include "../Widgets/CommandPointsHUDWidget.h"

ARTSHUD::ARTSHUD() {
	//static ConstructorHelpers::FClassFinder<UUserWidget> HealthBarObj(TEXT("/Game/FirstPerson/UI/Health_UI"));
	//HUDWidgetClass = HealthBarObj.Class;
}

void ARTSHUD::BeginPlay() {
	Super::BeginPlay();

	check(GEngine); GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, TEXT("Creating command points menu"));
	if (commandPointsMenuBP) {
		commandPointsMenu = CreateWidget<UCommandPointsHUDWidget>(GetWorld()->GetFirstPlayerController(), commandPointsMenuBP, "CommandPointsMenu");
		if (commandPointsMenu != nullptr)
		{
			commandPointsMenu->AddToViewport();
			check(GEngine); GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, TEXT("Command points menu added to viewport"));
		}
		else {
			check(GEngine); GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Command points menu bp not assigned!"));
		}
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
		previouslyFoundEntities.RemoveAllSwap([](AEntityPawn* a) {return a == nullptr; });

		//if (foundEntities.Num() > 0) {
		//	for (auto & entity : foundEntities) {
		//		entity->SetDeselected();
		//	}
		//}
		foundEntities.Empty();
		GetActorsInSelectionRectangle<AEntityPawn>(InitialSelectionPoint, CurrentSelectionPosition, foundEntities, false, false);

		for (int i = 0; i < foundEntities.Num(); ++i) { //remove all enemy entities
			if (foundEntities[i]->IsOwnedByPlayer())
				continue;
			foundEntities[i]->SetSelected(false);
			foundEntities.RemoveAtSwap(i);
		}

		//set found entitites which weren't selected previously to selected and remove them from previously found entities
		for (auto& entity : foundEntities) {
			if (previouslyFoundEntities.Find(entity) >= 0) {
				previouslyFoundEntities.Remove(entity);
			}
			else {
				entity->SetSelected(true);
			}
		}

		//deselect entities which were previously found and are no longer selected
		for (auto& entity : previouslyFoundEntities) {
			if (entity == nullptr)
				continue;

			entity->SetSelected(false);
		}
	}
}

FVector2D ARTSHUD::GetMousePosition2D()
{
	float posX, posY;
	GetOwningPlayerController()->GetMousePosition(posX, posY);
	return FVector2D(posX, posY);
}