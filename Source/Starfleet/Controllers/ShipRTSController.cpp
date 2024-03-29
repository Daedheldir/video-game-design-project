// Fill out your copyright notice in the Description page of Project Settings.

#include "ShipRTSController.h"
#include "../UI/HUD/RTSHUD.h"
#include "../InputStateInfo.h"
#include "../Entities/EntityPawn.h"

AShipRTSController::AShipRTSController() :
	cameraMovementScale{ 2000, 2000, 5000 },
	cameraMovementAccScale{ 4000, 4000, 5000 }
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
}

void AShipRTSController::BeginPlay()
{
	HUDptr = Cast<ARTSHUD>(GetHUD());
	TArray<AActor*> foundActors;

	InputState = GetWorld()->SpawnActor<AInputStateInfo>(AInputStateInfo::StaticClass());
	entityManager = GetWorld()->SpawnActor<AEntityManagerActor>(entityManager_BP_Class);
	entityManager->Tags.Add("PlayerEntityManager");

	enemyEntityManager = GetWorld()->SpawnActor<AEntityManagerActor>(
		enemyEntityManager_BP_Class,
		FTransform(FVector{ 50000,0,0 })
		);

	enemyEntityManager->SpawnRandomFleet();
}

void AShipRTSController::SetupInputComponent()
{
	Super::SetupInputComponent();

	//Entity spawning
	InputComponent->BindAction("SpawnEntity", IE_Released, this, &AShipRTSController::SpawnEntity);

	//Entity movement
	InputComponent->BindAction("LeftMouseClick", IE_Pressed, this, &AShipRTSController::SelectionPressed);
	InputComponent->BindAction("LeftMouseClick", IE_Released, this, &AShipRTSController::SelectionReleased);
	InputComponent->BindAction("RightMouseClick", IE_Released, this, &AShipRTSController::MoveReleased);

	//Camera movement
	InputComponent->BindAxis(TEXT("CameraMovementY"), this, &AShipRTSController::CameraMoveForward);
	InputComponent->BindAxis(TEXT("CameraMovementX"), this, &AShipRTSController::CameraMoveSide);
	InputComponent->BindAxis(TEXT("CameraMovementZ"), this, &AShipRTSController::CameraMoveHeight);
}
void AShipRTSController::SpawnEntity()
{
	EntityTypes selectedEntity = InputState->GetLastSelectedSpawnShipButton();
	SpawnEntity(selectedEntity);
}
void AShipRTSController::SpawnEntity(EntityTypes enitityType) {
	check(GEngine); GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::White, TEXT("Spawn Entity"));
	if (this->entityManager == nullptr) {
		check(GEngine); GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("EntityManager not found!"));
		return;
	}
	FHitResult hit;
	GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, hit);
	this->entityManager->SpawnEntityAtLocation(enitityType, hit.Location);
}
void AShipRTSController::SelectionPressed()
{
	switch (InputState->GetCurrentInputState())
	{
	case AInputStateInfo::InputState::NONE:
		check(GEngine); GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::White, TEXT("SelectionPressed, InputState::NONE"));

		HUDptr->bStartSelecting = true;
		HUDptr->InitialSelectionPoint = HUDptr->GetMousePosition2D();
		InputState->SetCurrentInputState(AInputStateInfo::InputState::SELECTING);
		break;
	case AInputStateInfo::InputState::SELECTING:
		check(GEngine); GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::White, TEXT("SelectionPressed, InputState::SELECTING"));

		break;
	case AInputStateInfo::InputState::PLACING_SHIP:
		check(GEngine); GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::White, TEXT("SelectionPressed, InputState::PLACING_SHIP"));

		break;
	default:
		break;
	}
}

void AShipRTSController::SelectionReleased()
{
	switch (InputState->GetCurrentInputState())
	{
	case AInputStateInfo::InputState::NONE:
		check(GEngine); GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::White, TEXT("SelectionReleased, InputState::NONE"));

		break;
	case AInputStateInfo::InputState::SELECTING:
		check(GEngine); GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::White, TEXT("SelectionReleased, InputState::SELECTING"));

		HUDptr->bStartSelecting = false;
		selectedEntities = HUDptr->foundEntities;
		InputState->SetCurrentInputState(AInputStateInfo::InputState::NONE);
		break;
	case AInputStateInfo::InputState::PLACING_SHIP:
		//spawn ship and go back to none
		check(GEngine); GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::White, TEXT("SelectionReleased, InputState::PLACING_SHIP"));

		SpawnEntity();
		InputState->SetCurrentInputState(AInputStateInfo::InputState::NONE);
		break;
	default:
		break;
	}
}

void AShipRTSController::MoveReleased()
{
	FHitResult hit;
	GetHitResultUnderCursor(ECollisionChannel::ECC_WorldDynamic, false, hit);
	AActor* hitActor = hit.GetActor();

	if(hitActor == nullptr)
		return;

	if (hitActor->GetClass()->IsChildOf(AEntityPawn::StaticClass())) {
		check(GEngine); GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::White, TEXT("Right Released at entity ") + hit.GetActor()->GetClass()->GetName());
		AEntityPawn* hitEntity = Cast<AEntityPawn>(hitActor);
		if(hitEntity != nullptr){
			if (!hitEntity->IsOwnedByPlayer()) { //if its enemy then make turrets target it
				for (int i = 0; i < selectedEntities.Num(); ++i) {
					selectedEntities[i]->CommandTurretsTarget(hitActor);
				}
			}
		}
	}
	else {
		for (int i = 0; i < selectedEntities.Num(); ++i) {
			AEntityPawn* entity = selectedEntities[i];
			if(entity == nullptr) continue;

			FVector actorOrgin, actorBoundsExtent;

			entity->GetActorBounds(true, actorOrgin, actorBoundsExtent);
			float locationSpacing = FMath::Max(actorBoundsExtent.X, actorBoundsExtent.Y) * 2.0f;

			FVector moveLocation = hit.Location + FVector(i / 2 * locationSpacing * 3, i % 2 * locationSpacing * 3, 0);
			entity->CommandMoveTo(moveLocation);

			DrawDebugSphere(GetWorld(), moveLocation, 50.0f, 16, FColor(255, 255, 255), false, 10.0f);
		}
		check(GEngine); 
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::White, TEXT("Move Released at ") + hit.Location.ToCompactString());
	}
}

void AShipRTSController::CameraMoveForward(float axisVal)
{
	if (axisVal == 0) {
		if (currentCameraMovementVelocity.X < 0)
			currentCameraMovementVelocity.X = 0;
		else
			currentCameraMovementVelocity.X -= cameraMovementAccScale.X * GetWorld()->GetDeltaSeconds();
	}
	else
		if (currentCameraMovementVelocity.X < cameraMovementScale.X)
			currentCameraMovementVelocity.X += cameraMovementAccScale.X * GetWorld()->GetDeltaSeconds();

	const FVector cameraMovementVec = this->GetPawn()->GetActorLocation().ForwardVector;
	const FRotator cameraRotation = this->GetPawn()->GetActorRotation();
	const FVector movementDir = (cameraRotation.RotateVector(cameraMovementVec) * FVector { 1, 1, 0 }).GetSafeNormal();

	const FVector newLocation = GetCameraMovementNewLocation(
		FVector(GetPawn()->GetActorLocation().Z / 100
			* movementDir * axisVal
			* currentCameraMovementVelocity.X).GetClampedToMaxSize(20000.0));
	this->GetPawn()->SetActorLocation(newLocation);
}

void AShipRTSController::CameraMoveSide(float axisVal)
{
	if (axisVal == 0) {
		if (currentCameraMovementVelocity.Y < 0)
			currentCameraMovementVelocity.Y = 0;
		else
			currentCameraMovementVelocity.Y -= cameraMovementAccScale.Y * GetWorld()->GetDeltaSeconds();
	}
	else
		if (currentCameraMovementVelocity.Y < cameraMovementScale.Y)
			currentCameraMovementVelocity.Y += cameraMovementAccScale.Y * GetWorld()->GetDeltaSeconds();

	const FVector cameraMovementVec = this->GetPawn()->GetActorLocation().RightVector;
	const FRotator cameraRotation = this->GetPawn()->GetActorRotation();
	const FVector movementDir = (cameraRotation.RotateVector(cameraMovementVec) * FVector { 1, 1, 0 }).GetSafeNormal();

	const FVector newLocation = GetCameraMovementNewLocation(
		FVector(GetPawn()->GetActorLocation().Z / 100
			* movementDir * axisVal
			* currentCameraMovementVelocity.Y).GetClampedToMaxSize(20000.0));	this->GetPawn()->SetActorLocation(newLocation);
}

void AShipRTSController::CameraMoveHeight(float axisVal)
{
	if (axisVal == 0) {
		if (currentCameraMovementVelocity.Z < 0)
			currentCameraMovementVelocity.Z = 0;
		else
			currentCameraMovementVelocity.Z -= cameraMovementAccScale.Z * GetWorld()->GetDeltaSeconds();
	}
	else
		if (currentCameraMovementVelocity.Z < cameraMovementScale.Z)
			currentCameraMovementVelocity.Z += cameraMovementAccScale.Z * GetWorld()->GetDeltaSeconds();

	const FVector movementDir = this->GetPawn()->GetActorLocation().UpVector;

	const FVector newLocation = GetCameraMovementNewLocation(
		FVector(GetPawn()->GetActorLocation().Z / 100
			* movementDir * axisVal
			* cameraMovementAccScale.Z).GetClampedToMaxSize(20000.0));
	this->GetPawn()->SetActorLocation(newLocation);
}

FVector AShipRTSController::GetCameraMovementNewLocation(const FVector& newLocationOffset) const
{
	const APawn* controlledPawn = this->GetPawn();
	const FVector previousLocation = controlledPawn->GetActorLocation();
	return previousLocation + newLocationOffset * GetWorld()->GetDeltaSeconds();
}