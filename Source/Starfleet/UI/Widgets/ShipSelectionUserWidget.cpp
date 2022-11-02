// Fill out your copyright notice in the Description page of Project Settings.

#include "ShipSelectionUserWidget.h"

void UShipSelectionUserWidget::PlaySelectedAnimation()
{
	if (!this->IsAnimationPlaying(selectedAnimation)) {
		if (this->IsAnimationPlaying(unselectedAnimation))
			this->StopAnimation(unselectedAnimation);

		this->PlayAnimation(selectedAnimation);
	}
}

void UShipSelectionUserWidget::PlayUnselectedAnimation()
{
	if (!this->IsAnimationPlaying(unselectedAnimation)) {
		if (this->IsAnimationPlaying(selectedAnimation))
			this->StopAnimation(selectedAnimation);

		this->PlayAnimation(unselectedAnimation);
	}
}

void UShipSelectionUserWidget::NativeConstruct()
{
}