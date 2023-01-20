// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/**
 *
 */
UENUM(BlueprintType)
enum class EntityTypes : uint8 {
	IMPERIAL = 0 UMETA(DisplayName = "Imperial"),
	INSURGENT = 1 UMETA(DisplayName = "Insurgent"),
	STRIKER = 2 UMETA(DisplayName = "Striker")
};