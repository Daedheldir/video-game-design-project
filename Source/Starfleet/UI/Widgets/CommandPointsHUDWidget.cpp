// Fill out your copyright notice in the Description page of Project Settings.

#include "CommandPointsHUDWidget.h"
#include <Internationalization/Text.h>
#include "../../InputStateInfo.h"
#include "../../Managers/EntityManagerActor.h"

void UCommandPointsHUDWidget::NativeConstruct() {
	Super::NativeConstruct();
	if (!spawnShip1Button->OnClicked.IsBound())
		spawnShip1Button->OnClicked.AddDynamic(this, &UCommandPointsHUDWidget::OnClickedSpawnStriker);
	if (!spawnShip2Button->OnClicked.IsBound())
		spawnShip2Button->OnClicked.AddDynamic(this, &UCommandPointsHUDWidget::OnClickedSpawnInsurgent);
	if (!spawnShip3Button->OnClicked.IsBound())
		spawnShip3Button->OnClicked.AddDynamic(this, &UCommandPointsHUDWidget::OnClickedSpawnImperial);

	spawnShip1Text->SetText(FText::FromString(L"Striker - 100"));
	spawnShip2Text->SetText(FText::FromString(L"Insurgent - 300"));
	spawnShip3Text->SetText(FText::FromString(L"Imperial - 500"));
}

void UCommandPointsHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime) {
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!PlayerEntityManager) {
		check(GEngine); GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("UCommandPointsHUDWidget::NativeTick PlayerEntityManager is null!"));
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
void UCommandPointsHUDWidget::OnClickedSpawnImperial() {
	OnClickedSpawnShip(EntityTypes::IMPERIAL);
}
void UCommandPointsHUDWidget::OnClickedSpawnInsurgent() {
	OnClickedSpawnShip(EntityTypes::INSURGENT);
}
void UCommandPointsHUDWidget::OnClickedSpawnStriker() {
	OnClickedSpawnShip(EntityTypes::STRIKER);
}
void UCommandPointsHUDWidget::OnClickedSpawnShip(EntityTypes entityType) {
	if (!InputStateInfo) {
		check(GEngine); GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("UCommandPointsHUDWidget::OnClicked InputStateInfo is null!"));
		InitializeInputStatePtr();
	}
	if (InputStateInfo) {
		InputStateInfo->SetCurrentInputState(AInputStateInfo::InputState::PLACING_SHIP);
		InputStateInfo->SetLastSelectedSpawnShipButton(entityType);
	}

	check(GEngine); GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, TEXT("Spawn S1 pressed"));
}