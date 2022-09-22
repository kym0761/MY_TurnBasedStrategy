// Copyright Epic Games, Inc. All Rights Reserved.

#include "TurnBaseStrategyPlayerController.h"
#include "GameFramework/Pawn.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "TurnBaseStrategyCharacter.h"
#include "Engine/World.h"

#include "Kismet/GameplayStatics.h"
//#include "GridSystem.h"

ATurnBaseStrategyPlayerController::ATurnBaseStrategyPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
}

void ATurnBaseStrategyPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	if(bInputPressed)
	{
		FollowTime += DeltaTime;

		// Look for the touch location
		FVector HitLocation = FVector::ZeroVector;
		FHitResult Hit;
		if(bIsTouch)
		{
			GetHitResultUnderFinger(ETouchIndex::Touch1, ECC_Visibility, true, Hit);
		}
		else
		{
			GetHitResultUnderCursor(ECC_Visibility, true, Hit);
		}
		HitLocation = Hit.Location;

		//AGridSystem* gridsystem = Cast<AGridSystem>(UGameplayStatics::GetActorOfClass(GetWorld(), AGridSystem::StaticClass()));
		//if (gridsystem)
		//{
		//	FGrid grid = gridsystem->WorldToGrid(Hit.Location);

		//	bool isvalid = gridsystem->IsValidGrid(grid);

		//	//UE_LOG(LogTemp, Warning, TEXT("%s"), *grid.ToString());
		//	//if (isvalid)
		//	//{
		//	//	UE_LOG(LogTemp, Warning, TEXT("Yes"));
		//	//}
		//	//else
		//	//{
		//	//	UE_LOG(LogTemp, Warning, TEXT("no....."));
		//	//}

		//	
		//	auto gridObj = gridsystem->GetValidGridObject(grid);

		//	//if (IsValid(gridObj))
		//	//{
		//	//	UE_LOG(LogTemp, Warning, TEXT("%s"), *gridObj->ToString());
		//	//}
		//	//else
		//	//{
		//	//	UE_LOG(LogTemp, Warning, TEXT("there is no such GridObj.."));
		//	//}

		//	gridsystem->HideAllGridVisual();
		//	gridsystem->ShowGridRange(grid, 3, EGridVisualType::Blue);



		//}


		// Direct the Pawn towards that location
		APawn* const MyPawn = GetPawn();
		if(MyPawn)
		{
			FVector WorldDirection = (HitLocation - MyPawn->GetActorLocation()).GetSafeNormal();
			MyPawn->AddMovementInput(WorldDirection, 1.f, false);
		}
	}
	else
	{
		FollowTime = 0.f;
	}
}

void ATurnBaseStrategyPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	InputComponent->BindAction("SetDestination", IE_Pressed, this, &ATurnBaseStrategyPlayerController::OnSetDestinationPressed);
	InputComponent->BindAction("SetDestination", IE_Released, this, &ATurnBaseStrategyPlayerController::OnSetDestinationReleased);

	// support touch devices 
	InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &ATurnBaseStrategyPlayerController::OnTouchPressed);
	InputComponent->BindTouch(EInputEvent::IE_Released, this, &ATurnBaseStrategyPlayerController::OnTouchReleased);

}

void ATurnBaseStrategyPlayerController::OnSetDestinationPressed()
{
	// We flag that the input is being pressed
	bInputPressed = true;
	// Just in case the character was moving because of a previous short press we stop it
	StopMovement();
}

void ATurnBaseStrategyPlayerController::OnSetDestinationReleased()
{
	// Player is no longer pressing the input
	bInputPressed = false;

	// If it was a short press
	if(FollowTime <= ShortPressThreshold)
	{
		// We look for the location in the world where the player has pressed the input
		FVector HitLocation = FVector::ZeroVector;
		FHitResult Hit;
		GetHitResultUnderCursor(ECC_Visibility, true, Hit);
		HitLocation = Hit.Location;

		// We move there and spawn some particles
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, HitLocation);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, HitLocation, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::None, true);
	}
}

void ATurnBaseStrategyPlayerController::OnTouchPressed(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	bIsTouch = true;
	OnSetDestinationPressed();
}

void ATurnBaseStrategyPlayerController::OnTouchReleased(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	bIsTouch = false;
	OnSetDestinationReleased();
}
