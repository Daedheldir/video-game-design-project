// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <Runtime/UMG/Public/Components/Button.h>
#include <Runtime/UMG/Public/Components/TextBlock.h>
#include <Kismet/GameplayStatics.h>

#include "CommandPointsHUDWidget.generated.h"

class AInputStateInfo;
class AEntityManagerActor;
/**
 *
 */
UCLASS()
class STARFLEET_API UCommandPointsHUDWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void InitializeInputStatePtr();
	void InitializePlayerEntityManagerPtr();

	UFUNCTION()
		void OnClicked();
private:
	template<class T>
	inline T* InitializePtrTemplate(const FString& tag)
	{
		TArray<AActor*> foundActors;
		UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), T::StaticClass(), FName(tag), foundActors);

		if (foundActors.Num() > 0) {
			if (foundActors.Num() > 1) {
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("UCommandPointsHUDWidget::InitializePtrTemplate found multiple actors with tag " + tag));
			}
			if (foundActors[0] != nullptr) {
				T* ptr = Cast<T>(foundActors[0]);
				return ptr;
			}
		}

		return nullptr;
	}
protected:
	AInputStateInfo* InputStateInfo;
	AEntityManagerActor* PlayerEntityManager;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UTextBlock* commandPointsText;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UButton* commandPointsButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UTextBlock* spawnShip1Text;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UButton* spawnShip1Button;
};
