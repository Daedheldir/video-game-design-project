// Fill out your copyright notice in the Description page of Project Settings.

#include "InputStateInfo.h"

AInputStateInfo::AInputStateInfo()
{
	this->Tags.Add({ "InputStateInfoObject" });
	inputState = InputState::NONE;
}

AInputStateInfo::InputState AInputStateInfo::GetCurrentInputState()
{
	return inputState;
}

void AInputStateInfo::SetCurrentInputState(InputState newState)
{
	inputState = newState;

	FString newStateName = "unknown";
	switch (newState)
	{
	case AInputStateInfo::InputState::NONE:
		newStateName = "NONE";
		break;
	case AInputStateInfo::InputState::SELECTING:
		newStateName = "SELECTING";
		break;
	case AInputStateInfo::InputState::PLACING_SHIP:
		newStateName = "PLACING_SHIP";
		break;
	}
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, TEXT("AInputStateInfo::SetCurrentInputState, new input state is " + newStateName));
}