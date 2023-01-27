// Fill out your copyright notice in the Description page of Project Settings.

#include "EntityPawn.h"
#include <Blueprint/UserWidget.h>
#include "../UI/Widgets/ShipSelectionUserWidget.h"
#include "NiagaraFunctionLibrary.h"
#include "TurretPawnBase.h"
#include <GameFramework/SpringArmComponent.h>
#include <Components/CapsuleComponent.h>
//#include <GeometryCollection/GeometryCollectionComponent.h>

// Sets default values
AEntityPawn::AEntityPawn() :
	shipStaticMesh{ CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ship Mesh")) },
	shipMoveComponent{ CreateDefaultSubobject<UShipMovementComponent>(TEXT("Ship Movement Component")) },
	SelectionSpringArm{ CreateDefaultSubobject<USpringArmComponent>(TEXT("Selection Spring Arm")) },
	shipSelectionWidget{ CreateDefaultSubobject<UWidgetComponent>(TEXT("Selection Widget Component"))
}
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	shipStaticMesh->SetupAttachment(RootComponent);

	GetMovementComponent()->SetPlaneConstraintNormal(FVector(0, 0, 1));
	GetMovementComponent()->SetPlaneConstraintEnabled(true);
	GetMovementComponent()->bSnapToPlaneAtStart = true;
	GetMovementComponent()->SetUpdatedComponent(this->GetRootComponent());

	SelectionSpringArm->SetupAttachment(shipStaticMesh);
	shipSelectionWidget->SetupAttachment(SelectionSpringArm);

	isSelected = false;

	fHealth = 100.0f;
}

void AEntityPawn::SetSelected(const bool selected)
{
	if (shipSelectionWidget == nullptr)
		return;
	if (shipSelectionWidget->GetWidget() == nullptr)
		return;

	UShipSelectionUserWidget* selectionWidget = Cast<UShipSelectionUserWidget>(shipSelectionWidget->GetWidget());
	check(GEngine);
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 3.f, FColor::White,
		FString::Printf(TEXT("Ship is now %s, request to %s"),
			isSelected ? TEXT("selected") : TEXT("unselected"),
			selected ? TEXT("select") : TEXT("unselect"))
	);

	if (selectionWidget == nullptr)
		return;

	if (selected && !IsCurrentlySelected()) {	//if entity isn't currently selected then play select animation
		selectionWidget->PlaySelectedAnimation();
	}
	else if (!selected && IsCurrentlySelected()) { //if entity is currently selected then play deselect animation
		selectionWidget->PlayUnselectedAnimation();
	}

	isSelected = selected;
}

bool AEntityPawn::IsCurrentlySelected() const
{
	return isSelected;
}

bool AEntityPawn::IsOwnedByPlayer() const
{
	return ownedByPlayer;
}

bool AEntityPawn::IsAlive() const
{
	return fHealth > 0.0f;
}

void AEntityPawn::CauseDamage(const float damageVal)
{
	fHealth -= damageVal;
}

void AEntityPawn::SetHealth(const float health)
{
	fHealth = health;
}

float AEntityPawn::GetHealth() const {
	return fHealth;
}

FVector AEntityPawn::GetCurrentVelocity() const
{
	UShipMovementComponent* movementComponent = Cast<UShipMovementComponent>(GetMovementComponent());
	if (movementComponent != nullptr)
		return movementComponent->GetCurrentSpeed();
	else
		return FVector::ZeroVector;
}

void AEntityPawn::CommandMoveTo(const FVector& destination) {
	moveTargetPosition = destination;
}

void AEntityPawn::CommandTurretsTarget(AActor* targetActor)
{
	for (int i = 0; i < SpawnedTurretArray.Num(); i++) {
		SpawnedTurretArray[i]->SetCurrentTurretTarget(targetActor);
	}
}

UPawnMovementComponent* AEntityPawn::GetMovementComponent() const {
	return shipMoveComponent;
}
#define ECC_FriendlyEntity ECollisionChannel::ECC_GameTraceChannel2
#define ECC_EnemyEntity ECollisionChannel::ECC_GameTraceChannel3
// Called when the game starts or when spawned
void AEntityPawn::BeginPlay()
{
	Super::BeginPlay();
	CommandMoveTo(this->GetActorLocation());
	FTimerHandle handle;
	FTimerDelegate SetDeselectedDelegate = FTimerDelegate::CreateUObject(this, &AEntityPawn::SetSelected, false);
	GetWorld()->GetTimerManager().SetTimer(handle, SetDeselectedDelegate, .1f, false);

	//engine particles
	if (engineParticleEffectBP != nullptr) {
		for (int i = 0; i < engineParticleEffectsPositions.Num(); ++i) {
			FTransform& transform = engineParticleEffectsPositions[i];
			UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
				engineParticleEffectBP,
				this->GetRootComponent(),
				FName(TEXT("Engine_particle_" + i)),
				FVector(transform.GetLocation()),
				FRotator(transform.GetRotation()),
				EAttachLocation::Type::KeepRelativeOffset,
				true);

			NiagaraComp->SetNiagaraVariableFloat(FString("engineSize"), 60);
			NiagaraComp->SetNiagaraVariableFloat(FString("acceleration"), 10);
			NiagaraComp->RegisterComponent();

			engineParticleEffects.Add(NiagaraComp);

			/*GEngine->AddOnScreenDebugMessage(
				-1,
				5.0f,
				FColor::Red,
				TEXT("Spawned engine system at " + NiagaraComp->GetComponentLocation().ToCompactString())
			);*/
		}
	}

	FillTurretSockets();

	if (shipStaticMesh) {
		if (IsOwnedByPlayer()) {
			shipStaticMesh->SetCollisionObjectType(ECC_FriendlyEntity);
			shipStaticMesh->SetCollisionProfileName("FriendlyEntity");
		}
		else {
			shipStaticMesh->SetCollisionObjectType(ECC_EnemyEntity);
			shipStaticMesh->SetCollisionProfileName("EnemyEntity");
		}
	}
}

// Called every frame
void AEntityPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!IsAlive()) {
		if (explosionEffectBP != nullptr) {
			UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				GetWorld(),
				explosionEffectBP,
				this->GetActorLocation());
			NiagaraComp->SetNiagaraVariableFloat(FString("ExplosionSize"), explosionSize);
		}

		DestroyTurrets();
		this->Destroy();

		if (DestructionGeometryActorBP != nullptr) {
			UWorld* World = GetWorld();
			check(World);
			FActorSpawnParameters spawnParams;
			spawnParams.Owner = this->GetOwner();

			AActor* DestructionActorWorld = World->SpawnActor<AActor>(
				DestructionGeometryActorBP->GetDefaultObject()->GetClass(),
				this->GetTransform(),
				spawnParams);
		}
	}

	FVector currentLocation = GetActorLocation();

	if (FVector::Dist2D(currentLocation, moveTargetPosition) > 200.0f) {
		FVector directionVec = { moveTargetPosition.X - currentLocation.X, moveTargetPosition.Y - currentLocation.Y, 0 };
		directionVec = directionVec.GetClampedToMaxSize(1.0f);

		if (GetMovementComponent() != nullptr) {
			GetMovementComponent()->AddInputVector(directionVec, true);
			for (auto niagaraComp : engineParticleEffects) {
				//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, TEXT("Command \"Move to\" given with vector ") + directionVec.ToCompactString());
				if (niagaraComp)
					niagaraComp->SetNiagaraVariableFloat(FString("acceleration"), 8.0f);
			}

			//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, TEXT("Command \"Move to\" given with vector ") + directionVec.ToCompactString());
		}
		else {
			check(GEngine); GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.0f, FColor::Red, TEXT("GetMovementComponent() returned nullptr!"));
		}
	}
	else {
		for (auto niagaraComp : engineParticleEffects) {
			if (niagaraComp)
				if (niagaraComp->IsActive()) {
					//niagaraComp->SetActive(false);
					niagaraComp->SetNiagaraVariableFloat(FString("acceleration"), 1.0f);
				}
		}
	}
}

// Called to bind functionality to input
void AEntityPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
}

void AEntityPawn::FillTurretSockets() {
	//GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.0f, FColor::White,TEXT("AEntityPawn::FillTurretSockets"));

	TurretArray = shipStaticMesh->GetAllSocketNames();
	for (int i = 0; i < TurretArray.Num(); i++) {
		SpawnTurret(TurretArray[i]);
	}
}
/** Spawns the turrets onto the sockets of the ship */
void AEntityPawn::SpawnTurret(const FName& _TurretSocketName) {
	//GEngine->AddOnScreenDebugMessage(INDEX_NONE, 25.0f, FColor::White,TEXT("Spawning turret at socket " + _TurretSocketName.ToString()));

	FTransform SocketTransform;

	// Make sure the world exists first
	UWorld* const World = GetWorld();

	if (World == nullptr || TurretBP == nullptr) {
		check(GEngine); GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.0f, FColor::Red,
			TEXT("World or TurretBP was null!"));
		return;
	}
	// Set the spawn parameters
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;

	ATurretPawnBase* RailGun_T = GetWorld()->SpawnActor<ATurretPawnBase>(
		TurretBP,
		SocketTransform,
		SpawnParams);

	RailGun_T->TurretLocalRot = SocketTransform.GetRotation().Rotator();
	RailGun_T->SetOwnedByPlayer(this->IsOwnedByPlayer());

	//TODO DIFFERENTIATE BETWEEN MIRRORED AND NOT MIRRORED TURRETS FOR CORRECT ANGLE HANDLING
	bool mirroredSocket = _TurretSocketName.GetPlainNameString().Contains(TEXT("_R")) ? true : false;
	if (mirroredSocket) {
		float tempMaxYaw = RailGun_T->TurretMaxYaw;
		RailGun_T->TurretMaxYaw = RailGun_T->TurretMinYaw;
		RailGun_T->TurretMinYaw = tempMaxYaw;
	}
	SpawnedTurretArray.Add(RailGun_T);

	if (RailGun_T != NULL) {
		RailGun_T->AttachToComponent(
			this->RootComponent,
			FAttachmentTransformRules::KeepRelativeTransform,
			_TurretSocketName);
	}
}
void AEntityPawn::DestroyTurrets() {
	for (int i = 0; i < SpawnedTurretArray.Num(); i++) {
		SpawnedTurretArray[i]->DetachFromControllerPendingDestroy();
		SpawnedTurretArray[i]->Destroy();
	}
}