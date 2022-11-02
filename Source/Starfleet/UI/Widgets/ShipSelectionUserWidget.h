// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ShipSelectionUserWidget.generated.h"

/**
 *
 */
UCLASS()
class STARFLEET_API UShipSelectionUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void PlaySelectedAnimation();
	void PlayUnselectedAnimation();

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UImage* selectionImage;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* selectedAnimation;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* unselectedAnimation;
};
