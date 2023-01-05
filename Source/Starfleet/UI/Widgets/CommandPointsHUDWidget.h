// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <Runtime/UMG/Public/Components/Button.h>
#include <Runtime/UMG/Public/Components/TextBlock.h>
#include "../../InputStateInfo.h"
//#include "CommonButtonBase.h"

#include "CommandPointsHUDWidget.generated.h"

/**
 *
 */
UCLASS()
class STARFLEET_API UCommandPointsHUDWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;
	void InitializeInputStatePtr();

	UFUNCTION()
		void OnClicked();

	AInputStateInfo* InputStateInfo;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UTextBlock* commandPointsText;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UButton* commandPointsButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UTextBlock* spawnShip1Text;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UButton* spawnShip1Button;
};
