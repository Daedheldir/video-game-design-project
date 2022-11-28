// Fill out your copyright notice in the Description page of Project Settings.

#include "EntityManagerActor.h"

// Sets default values
AEntityManagerActor::AEntityManagerActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Tags.Add("EntityManagerActor");
}

bool AEntityManagerActor::SpawnEntityAtLocation(const EntityTypes& entityType, const FVector& location)
{
	//TODO: Add entity factory
	AEntityPawn* newEntity = nullptr;
	FActorSpawnParameters spawnParams{};
	spawnParams.Owner = this;
	spawnParams.bNoFail = true;

	switch (entityType)
	{
	case EntityTypes::IMPERIAL:
		if (!entityBlueprints.Contains(EntityTypes::IMPERIAL)) {
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("Spawn Entity: Imperial not found in blueprints"));

			return false;
		}

		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::White, TEXT("Spawn Entity: Imperial at ") + location.ToCompactString());

		newEntity = GetWorld()->SpawnActor<AEntityPawn>(entityBlueprints[EntityTypes::IMPERIAL]->GetDefaultObject()->GetClass(), FVector(location.X, location.Y, 0), FRotator::ZeroRotator, spawnParams);
		if (newEntity == nullptr) {
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("Spawn Entity: Imperial SpawnActor returned null!"));
			return false;
		}
		break;
	default:
		return false;
	}
	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::White, TEXT("Spawn Entity: Adding entity ") + newEntity->GetName() + TEXT(" to array"));
	spawnedEntities.Add(newEntity);
	return true;
}

// Called when the game starts or when spawned
void AEntityManagerActor::BeginPlay()
{
}

// Called every frame
void AEntityManagerActor::Tick(float DeltaTime)
{
}