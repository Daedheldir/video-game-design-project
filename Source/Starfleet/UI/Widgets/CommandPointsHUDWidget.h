// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
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

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UTextBlock* commandPointsText;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UButton* commandPointsButton;
};
