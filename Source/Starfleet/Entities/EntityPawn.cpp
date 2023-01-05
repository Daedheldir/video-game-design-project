// Fill out your copyright notice in the Description page of Project Settings.

#include "EntityPawn.h"
#include <Blueprint/UserWidget.h>
#include "../UI/Widgets/ShipSelectionUserWidget.h"
#include "NiagaraFunctionLibrary.h"

// Sets default values
AEntityPawn::AEntityPawn() :
	shipStaticMesh{ CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ship Mesh")) },
	shipMoveComponent{ CreateDefaultSubobject<UShipMovementComponent>(TEXT("Ship Movement Component")) },
	shipSelectionWidget{ CreateDefaultSubobject<UWidgetComponent>(TEXT("Selection Widget Component")) }
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	shipStaticMesh->SetupAttachment(RootComponent);

	GetMovementComponent()->SetPlaneConstraintNormal(FVector(0, 0, 1));
	GetMovementComponent()->SetPlaneConstraintEnabled(true);
	GetMovementComponent()->bSnapToPlaneAtStart = true;
	GetMovementComponent()->SetUpdatedComponent(this->GetRootComponent());

	shipSelectionWidget->SetupAttachment(shipStaticMesh);
}

void AEntityPawn::SetSelected()
{
	UShipSelectionUserWidget* selectionWidget = Cast<UShipSelectionUserWidget>(shipSelectionWidget->GetWidget());
	selectionWidget->PlaySelectedAnimation();
}

void AEntityPawn::SetDeselected()
{
	UShipSelectionUserWidget* selectionWidget = Cast<UShipSelectionUserWidget>(shipSelectionWidget->GetWidget());
	selectionWidget->PlayUnselectedAnimation();
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

// Called when the game starts or when spawned
void AEntityPawn::BeginPlay()
{
	Super::BeginPlay();
	CommandMoveTo(this->GetActorLocation());
	FTimerHandle handle;
	GetWorld()->GetTimerManager().SetTimer(handle, this, &AEntityPawn::SetDeselected, .1f, false);

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
}

// Called every frame
void AEntityPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector currentLocation = GetActorLocation();

	if (FVector::Dist2D(currentLocation, moveTargetPosition) > 10.0f) {
		FVector directionVec = { moveTargetPosition.X - currentLocation.X, moveTargetPosition.Y - currentLocation.Y, 0 };
		directionVec = directionVec.GetClampedToMaxSize(1.0f);

		if (GetMovementComponent() != nullptr) {
			GetMovementComponent()->AddInputVector(directionVec, true);
			for (auto niagaraComp : engineParticleEffects) {
				//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, TEXT("Command \"Move to\" given with vector ") + directionVec.ToCompactString());

				niagaraComp->SetNiagaraVariableFloat(FString("acceleration"), 10.0f);
			}

			//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, TEXT("Command \"Move to\" given with vector ") + directionVec.ToCompactString());
		}
		else {
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.0f, FColor::Red, TEXT("GetMovementComponent() returned nullptr!"));
		}
	}
	else {
		for (auto niagaraComp : engineParticleEffects) {
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
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AEntityPawn::FillTurretSockets() {
	//GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.0f, FColor::White,TEXT("AEntityPawn::FillTurretSockets"));

	// We store all the socket names into our own Turret Array
	TurretArray = shipStaticMesh->GetAllSocketNames();
	// Then we iterate through that array and call our SpawnTurret() function and
	// pass it our array's index
	for (int i = 0; i < TurretArray.Num(); i++) {
		SpawnTurret(TurretArray[i]);
	}
}
/** Spawns the turrets onto the sockets of the ship */
void AEntityPawn::SpawnTurret(const FName& _TurretSocketName) {
	//GEngine->AddOnScreenDebugMessage(INDEX_NONE, 25.0f, FColor::White,TEXT("Spawning turret at socket " + _TurretSocketName.ToString()));

	// First we want to make sure these values are all zeroed out
	FTransform SocketTransform;

	// Make sure the world exists first
	UWorld* const World = GetWorld();

	if (World == nullptr || TurretBP == nullptr) {
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.0f, FColor::Red,
			TEXT("World or TurretBP was null!"));
		return;
	}
	// Set the spawn parameters
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;

	// This is where we instantiate our new Turret! RailGun_T can be whatever you want to name it
	ATurretPawnBase* RailGun_T = GetWorld()->SpawnActor<ATurretPawnBase>(
		TurretBP,
		SocketTransform,
		SpawnParams);

	// Assign our newly spawned turret its rotation
	RailGun_T->TurretLocalRot = SocketTransform.GetRotation().Rotator();

	//TODO DIFFERENTIATE BETWEEN MIRRORED AND NOT MIRRORED TURRETS FOR CORRECT ANGLE HANDLING
	bool mirroredSocket = _TurretSocketName.GetPlainNameString().Contains(TEXT("_R")) ? true : false;
	//IF socket is mirrored then flip the yaw constraints
	if (mirroredSocket) {
		float tempMaxYaw = RailGun_T->TurretMaxYaw;
		RailGun_T->TurretMaxYaw = RailGun_T->TurretMinYaw;
		RailGun_T->TurretMinYaw = tempMaxYaw;
	}
	// Now we add that newly spawned turret to our array of pointers so we can reference it later
	SpawnedTurretArray.Add(RailGun_T);

	// Now we make sure our turret isn't null, and then attach it to our socket's location.

	if (RailGun_T != NULL) {
		RailGun_T->AttachToComponent(
			this->RootComponent,
			FAttachmentTransformRules::KeepRelativeTransform,
			_TurretSocketName);
	}
}
void AEntityPawn::DestroyTurrets() {
	// Iterate over the turrets in the array we have them stored
	for (int i = 0; i < SpawnedTurretArray.Num(); i++) {
		SpawnedTurretArray[i]->DetachFromControllerPendingDestroy();
		SpawnedTurretArray[i]->Destroy();
	}
}