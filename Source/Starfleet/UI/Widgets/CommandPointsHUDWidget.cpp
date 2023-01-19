// Fill out your copyright notice in the Description page of Project Settings.

#include "CommandPointsHUDWidget.h"
#include <Internationalization/Text.h>
#include "../../InputStateInfo.h"
#include "../../Managers/EntityManagerActor.h"

void UCommandPointsHUDWidget::NativeConstruct() {
	Super::NativeConstruct();
	if (!spawnShip1Button->OnClicked.IsBound())
		spawnShip1Button->OnClicked.AddDynamic(this, &UCommandPointsHUDWidget::OnClicked);

	spawnShip1Text->SetText(FText::FromString(L"Spawn Ship 1"));
}

void UCommandPointsHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime) {
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!PlayerEntityManager) {
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("UCommandPointsHUDWidget::NativeTick PlayerEntityManager is null!"));
		InitializePlayerEntityManagerPtr();
	}
	if (PlayerEntityManager) {
		commandPointsText->SetText(FText::AsNumber(PlayerEntityManager->GetCurrentResourcesValue()));
	}
}

void UCommandPointsHUDWidget::InitializeInputStatePtr()
{
	InputStateInfo = InitializePtrTemplate<AInputStateInfo>("InputStateInfoObject");
}

void UCommandPointsHUDWidget::InitializePlayerEntityManagerPtr()
{
	PlayerEntityManager = InitializePtrTemplate<AEntityManagerActor>("PlayerEntityManager");
}

void UCommandPointsHUDWidget::OnClicked() {
	if (!InputStateInfo) {
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("UCommandPointsHUDWidget::OnClicked InputStateInfo is null!"));
		InitializeInputStatePtr();
	}
	if (InputStateInfo)
		InputStateInfo->SetCurrentInputState(AInputStateInfo::InputState::PLACING_SHIP);

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, TEXT("Spawn S1 pressed"));
}