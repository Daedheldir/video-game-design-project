// Fill out your copyright notice in the Description page of Project Settings.

#include "TurretPawnBase.h"
#include "NiagaraFunctionLibrary.h"
#include "MunitionsBase.h"
#include "Kismet/GameplayStatics.h"
#include "EntityPawn.h"

// Sets default values
ATurretPawnBase::ATurretPawnBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	// Create the static mesh component
	TurretBaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TurretBaseMesh"));
	TurretHullMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TurretHullMesh"));
	TurretBarrelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TurretBarrelMesh"));
	RootComponent = TurretBaseMesh;
	TurretHullMesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	TurretBarrelMesh->AttachToComponent(TurretHullMesh, FAttachmentTransformRules::KeepRelativeTransform);

	// Set default variables
	currentTime = 0.0f;
	refireDelay = 3.0f;
	MaxRange = 10000.0f;

	IsOccupied = false;
	IsFiring = false;

	TurretYawSpeed = 20.f;
	TurretMaxYaw = 180.f;
	TurretMinYaw = -180.f;

	TurretPitchSpeed = 20.f;
	TurretMaxPitch = 45.f;
	TurretMinPitch = -1.f;

	TurretLocalRot = FRotator(0.f, 0.f, 0.f);
	TurretYawRot = FRotator(0.f, 0.f, 0.f);
	TurretPitchRot = FRotator(0.f, 0.f, 0.f);

	IsTargeting = false;

	// Set this actor to call Tick() every frame. You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ownedByPlayer = true;
}

void ATurretPawnBase::BeginPlay()
{
	Super::BeginPlay();
	CurrentYawMax = (TurretHullMesh->GetRelativeTransform().GetRotation().X) + TurretMaxYaw;
	CurrentYawMin = (TurretHullMesh->GetRelativeTransform().GetRotation().X) + TurretMinYaw;

	if (DEBUG_MODE) {
		GetMuzzleSockets();
	}

	refireDelay += (rand() % (int)(refireDelay * 10)) / 100.0f;
}

// Called every frame
void ATurretPawnBase::Tick(float DeltaTime) {
	if (CurrentTurretTarget != nullptr) {
		if (CurrentTurretTarget->IsActorBeingDestroyed()) {
			SetCurrentTurretTarget(nullptr);
			IsTargeting = false;
		}
	}
	TurretLookAt();
	if (currentTime < refireDelay)
		currentTime += DeltaTime;

	PrimaryFireControl();

	Super::Tick(DeltaTime);
}
bool ATurretPawnBase::IsOwnedByPlayer() const {
	return ownedByPlayer;
}
void ATurretPawnBase::SetOwnedByPlayer(bool owned) {
	this->ownedByPlayer = owned;
}

void ATurretPawnBase::SetCurrentTurretTarget(AActor* newTarget)
{
	CurrentTurretTarget = newTarget;
}

void ATurretPawnBase::TurretLookAt() {
	//GEngine->AddOnScreenDebugMessage(INDEX_NONE, 0.1f, FColor::White,
	//	FString::Printf(TEXT("Turret current yaw is %f, Max and Min Yaw = %f , %f"),
	//		CurrentTurretRot.Yaw,
	//		CurrentYawMax,
	//		CurrentYawMin));

	if (CurrentTurretTarget == nullptr) {
		return;
	}

	// Get Camera Vector & Rotator
	FVector TargetLocation = CurrentTurretTarget->GetTargetLocation(this);
	FVector TargetMovementDirection = CurrentTurretTarget->GetTransform().GetRotation().GetForwardVector();
	AEntityPawn* entity = Cast<AEntityPawn>(CurrentTurretTarget);
	FVector TargetVelocity = entity != nullptr ? entity->GetCurrentVelocity() : FVector::ZeroVector;

	//check(GEngine);
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, TargetVelocity.ToCompactString());

	// Get Turrets Vector & Rotator
	FVector TurretLocation = GetActorLocation();
	FRotator TurretRotationWorld = TurretHullMesh->GetComponentRotation();

	float distanceToTarget = FVector::Dist(TurretLocation, TargetLocation);
	float timeToTarget = distanceToTarget / 6000.0f;

	FVector PredictedTargetLocation = TargetLocation + TargetVelocity * timeToTarget;
	FVector TargetSize, TargetOrigin;
	CurrentTurretTarget->GetActorBounds(true, TargetOrigin, TargetSize);
	PredictedTargetLocation += FMath::VRand() * TargetSize * AccuracyDeviation;

	// Handle Turret Rotation
	FVector TurretToTarget = PredictedTargetLocation - TurretLocation;
	FVector TurretToTargetLocal = TurretRotationWorld.UnrotateVector(TurretToTarget);
	FRotator TurretTargetLookDirection = TurretToTargetLocal.Rotation();
	FRotator TurretYaw = FRotator(0, TurretTargetLookDirection.Yaw, 0);
	CurrentTurretRot = FRotator(TurretHullMesh->GetRelativeTransform().GetRotation());

	//DEBUG DRAWING
	//DrawDebugDirectionalArrow(GetWorld(), TurretLocation, TurretLocation + TurretToTarget, 25.0f, FColor::Green, false, 0.1f);
	//DrawDebugDirectionalArrow(GetWorld(), TurretLocation, TurretLocation + 150 * TurretMaxYawRotator.Vector(), 25.0f, FColor::Red, false, 0.1f);
	//DrawDebugDirectionalArrow(GetWorld(), TurretLocation, TurretLocation + 150 * TurretMinYawRotator.Vector(), 25.0f, FColor::Red, false, 0.1f);

	// We check to see if the turret is within it's movement range,
	//if it is, we add the rotation and do one last addition check.
	//GEngine->AddOnScreenDebugMessage(INDEX_NONE, 0.1f, FColor::White,
	//	FString::Printf(TEXT("Turret commanded yaw is %s , target is %s"),
	//		*TurretYaw.ToCompactString(),
	//		*TurretTargetLookDirection.ToCompactString())
	//);

	if (!FMath::IsWithin(CurrentTurretRot.Yaw, CurrentYawMin, CurrentYawMax)) {
		// turret can't aim at target
		IsTargeting = false;
		CurrentTurretRot.Yaw = FMath::Clamp(CurrentTurretRot.Yaw, CurrentYawMin + 0.01, CurrentYawMax - 0.01);
		return;
	}
	RootComponent->AddRelativeRotation(TurretYaw);

	// Now it's time to do the same for the turret's barrels pitch!
	FRotator TurretBarrelPitch = FRotator(TurretTargetLookDirection.Pitch, 0, 0);
	TurretBarrelPitch.Pitch = FMath::Clamp(TurretBarrelPitch.Pitch, TurretMinPitch, TurretMaxPitch);
	TurretBarrelMesh->SetRelativeRotation(TurretBarrelPitch);

	// Do the targeting check with the barrels as well!
	if (FMath::IsNearlyEqual(TurretBarrelPitch.Pitch, TurretTargetLookDirection.Pitch, 0.1)
		&& FMath::IsNearlyEqual(TurretYaw.Yaw, TurretTargetLookDirection.Yaw, 0.1))
	{
		IsTargeting = true;
	}
	else {
		IsTargeting = false;
	}
}

/* ITurretInterface */
// This acts as a gate for firing any projectiles.
// So if the turrets cannot see your cameras focal point they wont fire!
void ATurretPawnBase::PrimaryFireControl() {
	if (IsTargeting && currentTime >= refireDelay) {
		if (FVector::Distance(this->GetActorLocation(), CurrentTurretTarget->GetActorLocation()) <= MaxRange) {
			currentTime = 0.0f;
			GetMuzzleSockets();
		}
	}
}

// Same steps as to getting the sockets for the turrets!
void ATurretPawnBase::GetMuzzleSockets() {
	MuzzleArray = TurretBarrelMesh->GetAllSocketNames();
	for (int i = 0; i < MuzzleArray.Num(); i++) {
		SpawnMunitions(MuzzleArray[i]);
	}
}

// Almost the same as spawning the turrets, but this time we check to make sure the projectile blueprint exists
void ATurretPawnBase::SpawnMunitions(FName _MuzzleSocketName) {
	if (MunitionsBP) {
		// Get the location and the rotation for the new projectile
		FVector SpawnLocation = TurretBarrelMesh->GetSocketLocation(_MuzzleSocketName);
		FRotator SpawnRotation = TurretBarrelMesh->GetSocketRotation(_MuzzleSocketName);
		FTransform ProjectileTransform{ SpawnRotation , SpawnLocation };

		// Making sure the world exists, if it does we spawn it!
		UWorld* const World = GetWorld();
		if (World) {
			AMunitionsBase* Projectile = World->SpawnActorDeferred<AMunitionsBase>(
				MunitionsBP->GetDefaultObject()->GetClass(),
				ProjectileTransform,
				this->GetAttachParentActor(),
				this);

			// We give the projectiles an owner! This can be used later for hit detection and the like!
			Projectile->SetOwnedByPlayer(this->IsOwnedByPlayer());
			UGameplayStatics::FinishSpawningActor(Projectile, ProjectileTransform);
		}
		else {
			check(GEngine); GEngine->AddOnScreenDebugMessage(-1, 5.0f,
				FColor::Red,
				TEXT("ATurretPawnBase::SpawnMunitions - World for turrets is null!"));
		}

		// This is completely option. I made a muzzle flash for fun. You can remove the below If statement if you want.
		if (MunitionsParticleEffectBP) {
			UNiagaraFunctionLibrary::SpawnSystemAttached(
				MunitionsParticleEffectBP,
				TurretBarrelMesh,
				FName(TEXT("Muzzle_particle")),
				FVector::ZeroVector,
				FRotator::ZeroRotator,
				EAttachLocation::Type::SnapToTarget,
				true);
		}
		else {
			check(GEngine); GEngine->AddOnScreenDebugMessage(-1, 5.0f,
				FColor::Red,
				TEXT("ATurretPawnBase::SpawnMunitions - MunitionsParticleEffectBP for turrets is not assigned!"));
		}
		//DrawDebugSphere(GetWorld(), SpawnLocation, 25.0f, 16, FColor(255, 0, 0), false, 10.0f);
		//DrawDebugDirectionalArrow(GetWorld(), SpawnLocation, SpawnLocation + 50 * SpawnRotation.Vector(), 25.0f, FColor::Red, false, 10.0f);
	}
}