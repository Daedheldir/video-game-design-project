// Fill out your copyright notice in the Description page of Project Settings.

#include "CommandPointsHUDWidget.h"
#include <Internationalization/Text.h>
#include <Kismet/GameplayStatics.h>
void UCommandPointsHUDWidget::NativeConstruct() {
	Super::NativeConstruct();
	if (!spawnShip1Button->OnClicked.IsBound())
		spawnShip1Button->OnClicked.AddDynamic(this, &UCommandPointsHUDWidget::OnClicked);

	spawnShip1Text->SetText(FText::FromString(L"Spawn S1"));

	InitializeInputStatePtr();
}
void UCommandPointsHUDWidget::InitializeInputStatePtr()
{
	TArray<AActor*> foundActors;
	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), AInputStateInfo::StaticClass(), FName("InputStateInfoObject"), foundActors);

	if (foundActors.Num() > 0) {
		if (foundActors[0] != nullptr)
			InputStateInfo = Cast<AInputStateInfo>(foundActors[0]);
	}
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