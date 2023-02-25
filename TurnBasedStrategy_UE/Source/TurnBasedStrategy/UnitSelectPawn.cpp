// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitSelectPawn.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Kismet/KismetMathLibrary.h" //UKismetMathLibrary
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UnitCore/UnitCharacter.h"

#include "UnitAction/UnitMoveActionComponent.h"
#include "Manager/GridManager.h"
#include "Grid/Grid.h"
#include "Kismet/GameplayStatics.h"
#include "UMG/UnitActionListWidget.h"
#include "UMG/MainCanvasWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/CanvasPanel.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values
AUnitSelectPawn::AUnitSelectPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PawnMovement = CreateDefaultSubobject<UFloatingPawnMovement>("PawnMovement");
}

// Called when the game starts or when spawned
void AUnitSelectPawn::BeginPlay()
{
	Super::BeginPlay();
	
	MainCanvasWidget = CreateWidget<UMainCanvasWidget>(GetWorld(), UMainCanvasWidgetClass);
	MainCanvasWidget->AddToViewport();

	OnSelectedActionChanged.AddDynamic(this, &AUnitSelectPawn::OnSelectedActionChangedFunc);
	OnSelectedUnitChanged.AddDynamic(this, &AUnitSelectPawn::OnSelectedUnitChangedFunc);
	OnBusyChanged.AddDynamic(this, &AUnitSelectPawn::OnBusyChangedFunc);

	//GetWorldTimerManager().SetTimer(TraceTimer, this, &AUnitSelectPawn::UnitLook, 0.12f, true);

}

// Called every frame
void AUnitSelectPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AUnitSelectPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AUnitSelectPawn::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AUnitSelectPawn::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &AUnitSelectPawn::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &AUnitSelectPawn::LookUp);

	PlayerInputComponent->BindAction("UnitSelect", IE_Pressed, this, &AUnitSelectPawn::HandleSelectAction);

}

void AUnitSelectPawn::MoveForward(float Value)
{
	FVector cameraLocation;
	FRotator cameraRotation;
	GetController()->GetPlayerViewPoint(cameraLocation, cameraRotation);

	FRotator controlRot = cameraRotation;
	FVector temp = UKismetMathLibrary::GetForwardVector(controlRot);

	AddMovementInput(temp, Value);
}

void AUnitSelectPawn::MoveRight(float Value)
{
	FVector cameraLocation;
	FRotator cameraRotation;
	GetController()->GetPlayerViewPoint(cameraLocation, cameraRotation);

	FRotator controlRot = cameraRotation;
	FVector temp = UKismetMathLibrary::GetRightVector(controlRot);

	AddMovementInput(temp, Value);
}

void AUnitSelectPawn::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void AUnitSelectPawn::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

void AUnitSelectPawn::HandleSelectAction()
{
	if (bIsBusy)
	{
		return;
	}

	switch (PawnMode)
	{
	case EPawnMode::Selection:

		DoSelection();

		break;
	case EPawnMode::Action:

		DoAction();

		break;
	default:

		break;
	}


}

bool AUnitSelectPawn::TraceToGrid(FHitResult& OutHit)
{

	APlayerController* playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!IsValid(playerController))
	{
		UE_LOG(LogTemp, Warning, TEXT("false.. Player Controller Not Valid"));
		return false;
	}

	FVector loc;
	FVector rot;
	playerController->DeprojectMousePositionToWorld(loc, rot);

	TArray<TEnumAsByte<EObjectTypeQuery>> objects;

	objects.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));
	//objects.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));
	TArray<AActor*> ignores;
	//ignores.Add(this);

	//GetWorld()->LineTraceSingleByChannel(hit, loc, loc + rot * 1000, ECollisionChannel::ECC_Visibility);
	bool result = UKismetSystemLibrary::LineTraceSingleForObjects(
		GetWorld(),
		loc,
		loc + rot * 10000,
		objects,
		true,
		ignores,
		EDrawDebugTrace::None,
		//EDrawDebugTrace::ForDuration,
		OutHit,
		true,
		FLinearColor::Red,
		FLinearColor::Blue,
		5.0f
	);


	return result;
}

void AUnitSelectPawn::OnTracedGridChanged()
{

	AGridManager* gridManager = AGridManager::GetGridManager();
	if (!IsValid(gridManager))
	{
		UE_LOG(LogTemp, Warning, TEXT("Action Failed Cause GridManager Can't be found"));
		return;
	}

	if (IsValid(SelectedAction))
	{
		auto gridArray = SelectedAction->GetValidActionGridArray();
		if (gridArray.Contains(CurrentTracedGrid))
		{
			FVector unitLoc = SelectedAction->GetOwner()->GetActorLocation();
			FVector hitLoc = gridManager->GridToWorld(CurrentTracedGrid);
			hitLoc.Z = unitLoc.Z;
			FRotator look = UKismetMathLibrary::FindLookAtRotation(unitLoc, hitLoc);

			SelectedAction->GetOwner()->SetActorRotation(look);
		}
	}
}

void AUnitSelectPawn::DoSelection()
{
	bool unitSelected = TryUnitSelect();

	if (unitSelected) // 유닛이 설정되었을 때
	{
		if (!IsValid(SelectedUnit))
		{
			UE_LOG(LogTemp, Warning, TEXT("true.. Selected Unit Not Valid"));
			return;
		}

		if (IsValid(UnitActionsWidget))
		{
			UnitActionsWidget->RemoveFromParent();
		}

		FVector2D screenPos;
		FVector unitLocation = SelectedUnit->GetActorLocation();
		APlayerController* playerController = Cast<APlayerController>(GetController());
		bool bisOK = UGameplayStatics::ProjectWorldToScreen(
			playerController,
			unitLocation, 
			screenPos,
			true);

		//if (bisOK)
		//{
		//	UE_LOG(LogTemp, Warning, TEXT("projection ok"));
		//}
		//else
		//{
		//	UE_LOG(LogTemp, Warning, TEXT("projection failed"));
		//}

		if (IsValid(UnitActionsWidget))
		{
			UnitActionsWidget->RemoveFromParent();
			UnitActionsWidget = nullptr;
		}

		UnitActionsWidget = CreateWidget<UUnitActionListWidget>(GetWorld(), UnitActionListWidgetClass);
		MainCanvasWidget->MainCanvas->AddChild(UnitActionsWidget);
		UnitActionsWidget->InitUnitActionsWidget(SelectedUnit);

		UCanvasPanelSlot* canvasSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(UnitActionsWidget);
		if (IsValid(canvasSlot))
		{
			//UE_LOG(LogTemp, Warning, TEXT("unitLocation : %f, %f, %f"), unitLocation.X,unitLocation.Y,unitLocation.Z);
			//UE_LOG(LogTemp, Warning, TEXT("screenPos : %f, %f"), screenPos.X, screenPos.Y);
			canvasSlot->SetPosition(screenPos);
			//UE_LOG(LogTemp, Warning, TEXT("canvasSlot position : %f, %f"), canvasSlot->GetPosition().X, canvasSlot->GetPosition().Y);

			canvasSlot->SetSize(FVector2D(300, 500));
			//canvasSlot->SetAlignment(FVector2D(0.5f, 0.5f));
		}

	}
}

void AUnitSelectPawn::DoAction()
{
	if (!IsValid(SelectedUnit))
	{
		UE_LOG(LogTemp, Warning, TEXT("false.. Selected Unit Not Valid"));
		return;
	}

	if (!IsValid(SelectedAction))
	{
		UE_LOG(LogTemp, Warning, TEXT("false.. Selected Action Not Valid"));
		return;
	}

	FHitResult hit;
	bool result = TraceToGrid(hit);

	if (result)
	{
		FVector hitLocation = hit.Location;

		AGridManager* gridManager = AGridManager::GetGridManager();
		if (!IsValid(gridManager))
		{
			UE_LOG(LogTemp, Warning, TEXT("Action Failed Cause GridManager Can't be found"));
			return;
		}

		FGrid grid = gridManager->WorldToGrid(hitLocation);

		if (!gridManager->IsValidGrid(grid))
		{
			return;
		}

		SelectedAction->DealWithGridBeforeAction(grid);
	}
}

void AUnitSelectPawn::SetPawnMode(EPawnMode InputMode)
{
	PawnMode = InputMode;
}

bool AUnitSelectPawn::TryUnitSelect()
{

	APlayerController* playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!IsValid(playerController))
	{
		return false;
	}

	FVector loc;
	FVector rot;
	playerController->DeprojectMousePositionToWorld(loc, rot);

	TArray<TEnumAsByte<EObjectTypeQuery>> objects;

	objects.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));

	//////objects.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));
	//////objects.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));
	////objects.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	////objects.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_PhysicsBody));


	TArray<AActor*> ignores;
	//ignores.Add(this);

	FHitResult hit;

	bool result = UKismetSystemLibrary::LineTraceSingleForObjects(
		GetWorld(),
		loc,
		loc + rot * 10000,
		objects,
		true,
		ignores,
		EDrawDebugTrace::None,
		//EDrawDebugTrace::ForDuration,
		hit,
		true,
		FLinearColor::Red,
		FLinearColor::Blue,
		5.0f
	);

	if (result)
	{
		FVector hitLocation = hit.Location;
		AGridManager* gridManager = AGridManager::GetGridManager();
		if (!IsValid(gridManager))
		{
			UE_LOG(LogTemp, Warning, TEXT("GridManager Not Valid"));
			return false;
		}

		FGrid hitGrid = gridManager->WorldToGrid(hitLocation);
		if (gridManager->IsValidGrid(hitGrid))
		{
			auto gridUnitArr = gridManager->GetUnitArrayAtGrid(hitGrid);

			if (gridUnitArr.Num() == 0)
			{
				UE_LOG(LogTemp, Warning, TEXT("Unit Num 0"));
				return false;
			}

			if (IsValid(SelectedUnit) && SelectedUnit == gridUnitArr[0] )
			{
				UE_LOG(LogTemp, Warning, TEXT("Same Unit"));
				return true;
			}

			if (  !(gridUnitArr[0]->IsThisUnitCanAction()))
			{
				UE_LOG(LogTemp, Warning, TEXT("this Unit Can't Do AnyThing."));
				return false;
			}

			if (gridUnitArr[0]->ActorHasTag(FName("MyUnit")))
			{
				UE_LOG(LogTemp, Warning, TEXT("Unit Set!"));
				SetSelectUnit(gridUnitArr[0]);
				return true;
			}
		}
		
	}

	UE_LOG(LogTemp, Warning, TEXT("Nothing Unit Detected"));
	return false;
}

void AUnitSelectPawn::SetSelectUnit(AUnitCharacter* InputUnit)
{
	if (!IsValid(InputUnit)
		|| !IsValid(InputUnit) ||
		SelectedUnit == InputUnit)
	{
		return;
	}

	SelectedUnit = InputUnit;

	if (OnSelectedUnitChanged.IsBound())
	{
		OnSelectedUnitChanged.Broadcast();
	}
		

}

void AUnitSelectPawn::SetSelectedAction(UUnitActionComponent* InputUnitAction)
{
	if (SelectedAction == InputUnitAction)
	{
		return;
	}

	SelectedAction = InputUnitAction;

	if (OnSelectedActionChanged.IsBound())
	{
		OnSelectedActionChanged.Broadcast();
	}

	if (SelectedAction->OnActionSelected.IsBound())
	{
		SelectedAction->OnActionSelected.Broadcast();
	}

}

AUnitCharacter* AUnitSelectPawn::GetSelectedUnit()
{
	return SelectedUnit;
}

UUnitActionComponent* AUnitSelectPawn::GetSelectedAction()
{
	return SelectedAction;
}

AUnitSelectPawn* AUnitSelectPawn::GetUnitSelectPawn()
{
	if (!IsValid(GEngine) || !IsValid(GEngine->GameViewport))
	{
		return nullptr;
	}

	UWorld* world = GEngine->GameViewport->GetWorld();
	AUnitSelectPawn* unitSelectPawn = Cast<AUnitSelectPawn>(UGameplayStatics::GetActorOfClass(world, AUnitSelectPawn::StaticClass()));

	return unitSelectPawn;
}

bool AUnitSelectPawn::GetIsBusy() const
{
	return bIsBusy;
}

void AUnitSelectPawn::SetIsBusy(bool InputBool)
{
	if (bIsBusy == InputBool)
	{
		return;
	}

	bIsBusy = InputBool;
	if (OnBusyChanged.IsBound())
	{
		OnBusyChanged.Broadcast(InputBool);
	}
}

void AUnitSelectPawn::DeSelect()
{
	SelectedUnit = nullptr;
	SelectedAction = nullptr;

	if (OnSelectedActionChanged.IsBound())
	{
		OnSelectedActionChanged.Broadcast();
	}

	if (OnSelectedUnitChanged.IsBound())
	{
		OnSelectedUnitChanged.Broadcast();
	}
}

void AUnitSelectPawn::OnSelectedActionChangedFunc()
{
	AGridManager* gridManager = AGridManager::GetGridManager();
	if (IsValid(gridManager))
	{
		gridManager->RemoveAllGridVisual();
	}
}

void AUnitSelectPawn::OnSelectedUnitChangedFunc()
{
	AGridManager* gridManager = AGridManager::GetGridManager();
	if (IsValid(gridManager))
	{
		gridManager->RemoveAllGridVisual();
	}
}

void AUnitSelectPawn::OnBusyChangedFunc(bool InputBool)
{

}

void AUnitSelectPawn::UnitLook()
{
	if (PawnMode == EPawnMode::Action)
	{
		FHitResult hit;
		bool result = TraceToGrid(hit);

		if (result)
		{
			FVector hitLocation = hit.Location;

			AGridManager* gridManager = AGridManager::GetGridManager();
			if (!IsValid(gridManager))
			{
				UE_LOG(LogTemp, Warning, TEXT("Action Failed Cause GridManager Can't be found"));
				return;
			}

			FGrid grid = gridManager->WorldToGrid(hitLocation);

			if (!gridManager->IsValidGrid(grid))
			{
				return;
			}

			if (CurrentTracedGrid != grid)
			{
				CurrentTracedGrid = grid;
				OnTracedGridChanged();
			}
		}
	}
}

