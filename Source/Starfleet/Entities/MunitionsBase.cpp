// Fill out your copyright notice in the Description page of Project Settings.

#include "MunitionsBase.h"
#include "NiagaraFunctionLibrary.h"
#include <NiagaraComponent.h>
#include "../Entities/EntityPawn.h"

// Sets default values
AMunitionsBase::AMunitionsBase()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (!RootComponent)
	{
		RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSceneComponent"));
	}

	if (!CollisionComponent)
	{
		// Use a sphere as a simple collision representation.
		CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
		// Set the sphere's collision radius.
		CollisionComponent->InitSphereRadius(15.0f);

		CollisionComponent->BodyInstance.SetCollisionProfileName(TEXT("Projectile"));

		// Set the root component to be the collision component.
		RootComponent = CollisionComponent;
	}

	if (!ProjectileMovementComponent)
	{
		// Use this component to drive this projectile's movement.
		ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
		ProjectileMovementComponent->SetUpdatedComponent(CollisionComponent);
		ProjectileMovementComponent->InitialSpeed = 3000.0f;
		ProjectileMovementComponent->MaxSpeed = 3000.0f;
		ProjectileMovementComponent->bRotationFollowsVelocity = true;
		ProjectileMovementComponent->bShouldBounce = false;
		ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
	}

	explosionSize = 1.0f;
}

void AMunitionsBase::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f,
		FColor::White,
		TEXT("AMunitionsBase::OnHit - Projectile hit a ") + OtherActor->GetName());

	if (OtherActor != this && OtherComponent->IsSimulatingPhysics())
	{
		OtherComponent->AddImpulseAtLocation(ProjectileMovementComponent->Velocity * 100.0f, Hit.ImpactPoint);
	}
	if (OtherActor->IsA(AEntityPawn::StaticClass())) {
		AEntityPawn* entity = Cast<AEntityPawn>(OtherActor);
		entity->CauseDamage(20.0f);
	}
	UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		GetWorld(),
		explosionEffectBP,
		this->GetActorLocation());
	NiagaraComp->SetNiagaraVariableFloat(FString("ExplosionSize"), explosionSize);

	Destroy();
}

// Called when the game starts or when spawned
void AMunitionsBase::BeginPlay()
{
	Super::BeginPlay();
	if (CollisionComponent) {
		CollisionComponent->OnComponentHit.AddDynamic(this, &AMunitionsBase::OnHit);
	}
}

// Called every frame
void AMunitionsBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}