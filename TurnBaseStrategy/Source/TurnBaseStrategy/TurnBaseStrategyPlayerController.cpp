// Copyright Epic Games, Inc. All Rights Reserved.

#include "TurnBaseStrategyPlayerController.h"
#include "GameFramework/Pawn.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "TurnBaseStrategyCharacter.h"
#include "Engine/World.h"

#include "Kismet/GameplayStatics.h"
#include "GridManager.h"


#include "DrawDebugHelpers.h"// draw line
#include "GridObject.h"

ATurnBaseStrategyPlayerController::ATurnBaseStrategyPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
}

void ATurnBaseStrategyPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	APawn* const pawn = GetPawn();
	FGrid pawnGrid = FGrid(0,0);
	if (pawn)
	{
		FVector pawnLocation = pawn->GetActorLocation();
		AGridManager* gridManager = Cast<AGridManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGridManager::StaticClass()));
		if (gridManager)
		{
			pawnGrid = gridManager->WorldToGrid(pawnLocation);
			//UE_LOG(LogTemp, Warning, TEXT("PawnGrid : %s"), *pawnGrid.ToString());
		}
	}

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

		AGridManager* gridManager = Cast<AGridManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGridManager::StaticClass()));
		if (gridManager)
		{
			FGrid grid = gridManager->WorldToGrid(Hit.Location);

			bool isvalid = gridManager->IsValidGrid(grid);

			//UE_LOG(LogTemp, Warning, TEXT("%s"), *grid.ToString());
			if (isvalid)
			{
				UE_LOG(LogTemp, Warning, TEXT("Valid Grid"));
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Not Valid Grid"));
				return;
			}

			
			auto gridObj = gridManager->GetValidGridObject(grid);

			if (IsValid(gridObj))
			{
				UE_LOG(LogTemp, Warning, TEXT("%s"), *gridObj->ToString());
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("there is no such GridObj.."));
			}

			gridManager->RemoveAllGridVisual();
			gridManager->ShowGridRange(grid, 1, EGridVisualType::Blue);

			int32 pathLength;
			auto path = gridManager->FindPath(pawnGrid, grid, pathLength);
			if (path.Num() == 0)
			{
				UE_LOG(LogTemp, Warning, TEXT("Grid path is zero"));
				return;
			}
			UE_LOG(LogTemp, Warning, TEXT("target grid : %s"), *grid.ToString());
			UE_LOG(LogTemp, Warning, TEXT("Path Length : %d "), pathLength);
			for (int i = 0; i < path.Num(); i++)
			{
				//UE_LOG(LogTemp, Warning, TEXT("path[%d] : %s"), i,*path[i].ToString());

				//FVector start = gridManager->GridToWorld(path[i]) + FVector(0, 0, 10);
				//FVector  end = gridManager->GridToWorld(path[i + 1]) + FVector(0, 0, 10);

				//DrawDebugLine(GetWorld(), start, end, FColor::Black, false, 5.0f, 0, 2.0f);
				DrawDebugSphere(GetWorld(), gridManager->GridToWorld(path[i]), 10, 12, FColor::Blue, false, 1.5f, 0, 2.0f);

			}
		}


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
