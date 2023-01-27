// Fill out your copyright notice in the Description page of Project Settings.

#include "EntityManagerActor.h"
#include "../Entities/EntityPawn.h"

// Sets default values
AEntityManagerActor::AEntityManagerActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Tags.Add("EntityManagerActor");

	startingResources = 0.0f;
}

bool AEntityManagerActor::SpawnEntityAtLocation(const EntityTypes& entityType, const FVector& location)
{
	//TODO: Add entity factory
	AEntityPawn* newEntity = nullptr;
	FActorSpawnParameters spawnParams{};
	spawnParams.Owner = this;
	spawnParams.bNoFail = true;

	if (!entityBlueprints.Contains(entityType)) {
		check(GEngine);
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("Spawn Entity: Entity not found in blueprints"));
		return false;
	}

	if (!entitySpawnCosts.Contains(entityType)) {
		check(GEngine);
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("Spawn Entity: Entity has no associated cost"));
		return false;
	}

	if (entitySpawnCosts[entityType] > currentResources) {
		check(GEngine);
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("Spawn Entity: not enough points to spawn entity"));
		return false;
	}

	check(GEngine);
	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::White, TEXT("Spawn Entity: at ") + location.ToCompactString());

	newEntity = GetWorld()->SpawnActor<AEntityPawn>(entityBlueprints[entityType]->GetDefaultObject()->GetClass(), FVector(location.X, location.Y, 0), FRotator::ZeroRotator, spawnParams);
	if (newEntity == nullptr) {
		check(GEngine);
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("Spawn Entity: Entity SpawnActor returned null!"));
		return false;
	}

	currentResources -= entitySpawnCosts[entityType];

	check(GEngine);
	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::White, TEXT("Spawn Entity: Adding entity ") + newEntity->GetName() + TEXT(" to array"));
	spawnedEntities.Add(newEntity);
	return true;
}

float AEntityManagerActor::GetCurrentResourcesValue() const {
	return currentResources;
}

void AEntityManagerActor::SpawnRandomFleet()
{
	//select random number of ships to spawn while leaving enough resources for the rest of the fleet
	TSet<EntityTypes> entityKeysSet;
	entitySpawnCosts.GetKeys(entityKeysSet);
	TArray<EntityTypes> entityKeys = entityKeysSet.Array();

	for (int i = 0; i < entitySpawnCosts.Num(); ++i) {
		float currentResourcesLeft = GetCurrentResourcesValue();

		EntityTypes currentType = entityKeys[i];

		//calculate min resources to spawn at least one of each type
		float minResourcesForFleet = 0.0f;
		for (int j = i + 1; j < entitySpawnCosts.Num(); ++j) {
			EntityTypes currentSubType = entityKeys[j];
			minResourcesForFleet += entitySpawnCosts[currentSubType];
		}

		//calculate how many ships can be spawned
		check(static_cast<int>(std::ceil(entitySpawnCosts[currentType])) != 0);
		int maxSpawnedShips =
			static_cast<int>(std::floor(currentResourcesLeft - minResourcesForFleet))
			/ static_cast<int>(std::ceil(entitySpawnCosts[currentType]));

		int shipsToSpawn = maxSpawnedShips;

		//if this is last ship available, then spawn as much as possible
		if (i < entitySpawnCosts.Num() - 1) {
			//select random number of ships between 1 and maxSpawnedShips
			shipsToSpawn = (FMath::Rand() % maxSpawnedShips) + 1;
		}

		//spawn ships
		FVector spawnLocation{ this->GetActorLocation() };
		spawnLocation += { -i * 1000.0f, 0.0f, 0.0f};

		for (int k = 0; k < shipsToSpawn; ++k) {
			SpawnEntityAtLocation(currentType, spawnLocation);
			spawnLocation += {0, 1000, 0};
		}
	}
}

// Called when the game starts or when spawned
void AEntityManagerActor::BeginPlay()
{
	currentResources = startingResources;
}

// Called every frame
void AEntityManagerActor::Tick(float DeltaTime)
{
}