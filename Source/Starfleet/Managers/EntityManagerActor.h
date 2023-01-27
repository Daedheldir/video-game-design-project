// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Entities/EEntityTypes.h"
#include "EntityManagerActor.generated.h"

class AEntityPawn;

UCLASS()
class STARFLEET_API AEntityManagerActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AEntityManagerActor();

	UFUNCTION(BlueprintCallable)
		bool SpawnEntityAtLocation(const EntityTypes& entityType, const FVector& location);

	float GetCurrentResourcesValue() const;

	void SpawnRandomFleet();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	//----------------ENTITIES-------------------
	UPROPERTY(Category = "Entities", EditDefaultsOnly, BlueprintReadWrite)
		TMap<EntityTypes, TSubclassOf<AEntityPawn>> entityBlueprints;
	UPROPERTY(Category = "Entities", EditDefaultsOnly, BlueprintReadWrite)
		TMap<EntityTypes, float> entitySpawnCosts;

	TArray<AEntityPawn*> spawnedEntities;

	//----------------RESOURCES-------------------
	UPROPERTY(Category = "Resources", EditDefaultsOnly, BlueprintReadWrite)
		float startingResources;
	float currentResources;
};
