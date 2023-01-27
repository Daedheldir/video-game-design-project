// Fill out your copyright notice in the Description page of Project Settings.

#include "MunitionsBase.h"
#include "NiagaraFunctionLibrary.h"
#include <NiagaraComponent.h>
#include "../Entities/EntityPawn.h"
#include <Components/PointLightComponent.h>

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
		CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
		CollisionComponent->InitSphereRadius(15.0f);

		this->SetRootComponent(CollisionComponent);
	}

	if (!projectileParticles) {
		projectileParticles = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Projectile Particles Component"));

		projectileParticles->SetupAttachment(GetRootComponent());
	}
	if (!pointLight) {
		pointLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("Point Light"));

		pointLight->SetupAttachment(GetRootComponent());
	}

	if (!ProjectileMovementComponent)
	{
		ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));

		ProjectileMovementComponent->SetUpdatedComponent(CollisionComponent);
		ProjectileMovementComponent->InitialSpeed = 3000.0f;
		ProjectileMovementComponent->MaxSpeed = 3000.0f;
		ProjectileMovementComponent->bRotationFollowsVelocity = true;
		ProjectileMovementComponent->bShouldBounce = false;
		ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
	}

	explosionSize = 1.0f;
	ownedByPlayer = true;
}

bool AMunitionsBase::IsOwnedByPlayer() const {
	return ownedByPlayer;
}
void AMunitionsBase::SetOwnedByPlayer(bool owned) {
	this->ownedByPlayer = owned;
}

void AMunitionsBase::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit)
{
	check(GEngine); GEngine->AddOnScreenDebugMessage(-1, 5.0f,
		FColor::White,
		TEXT("AMunitionsBase::OnHit - Projectile hit a ") + OtherActor->GetClass()->GetName());

	if (this->IsOwnedBy(OtherActor)) {
		return;
	}

	if (OtherActor->IsA(AEntityPawn::StaticClass())) {
		AEntityPawn* entity = Cast<AEntityPawn>(OtherActor);

		entity->CauseDamage(20.0f);
		check(GEngine); GEngine->AddOnScreenDebugMessage(-1, 5.0f,
			FColor::White,
			TEXT("AMunitionsBase::OnHit - OtherActor has " + FString::Printf(L"%.1f", entity->GetHealth())));
	}

	if (explosionEffectBP) {
		UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			explosionEffectBP,
			this->GetActorLocation());
		NiagaraComp->SetNiagaraVariableFloat(FString("ExplosionSize"), explosionSize);
	}
	//Make projectile invisible to keep the trail
	projectileParticles->Deactivate();
	FTimerHandle HideLightTimerHandle;
	FTimerDelegate HideLightDelegate;
	HideLightDelegate.BindLambda([&]()
		{
			pointLight->SetVisibility(false);
		});
	GetWorld()->GetTimerManager().SetTimer(HideLightTimerHandle, HideLightDelegate, 0.2f, false);

	FTimerHandle DestroyTimerHandle;
	FTimerDelegate DestroyDelegate;
	DestroyDelegate.BindLambda([&]()
		{
			Destroy();
		});
	GetWorld()->GetTimerManager().SetTimer(DestroyTimerHandle, DestroyDelegate, 4.0f, false);
}

#define ECC_FriendlyProjectile ECollisionChannel::ECC_GameTraceChannel4
#define ECC_EnemyProjectile ECollisionChannel::ECC_GameTraceChannel1
// Called when the game starts or when spawned
void AMunitionsBase::BeginPlay()
{
	Super::BeginPlay();
	if (CollisionComponent) {
		if (IsOwnedByPlayer()) {
			check(GEngine); GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.0f,
				FColor::White,
				TEXT("Projectile is owned by player"));

			CollisionComponent->SetCollisionObjectType(ECC_FriendlyProjectile);
			CollisionComponent->SetCollisionProfileName("FriendlyProjectile");
			pointLight->SetTemperature(12000.0f);
		}
		else {
			check(GEngine); GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.0f,
				FColor::White,
				TEXT("Projectile is owned by enemy"));
			CollisionComponent->SetCollisionObjectType(ECC_EnemyProjectile);
			CollisionComponent->SetCollisionProfileName("EnemyProjectile");
			pointLight->SetTemperature(1700.0f);
		}
		CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AMunitionsBase::OnHit);
	}
}

// Called every frame
void AMunitionsBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}