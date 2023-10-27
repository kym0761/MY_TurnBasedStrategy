// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitControlPawn.h"

#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"

#include "Manager/GridManager.h"
#include "Manager/TurnManager.h"
#include "UnitCore/Unit.h"

#include "UMG/UnitActionListWidget.h"
#include "UMG/MainCanvasWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/CanvasPanel.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "UnitAction/UnitActionComponent.h"

#include "DebugHelper.h"

// Sets default values
AUnitControlPawn::AUnitControlPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PawnRoot = CreateDefaultSubobject<USceneComponent>(TEXT("PawnRoot"));
	SetRootComponent(PawnRoot);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

}

// Called when the game starts or when spawned
void AUnitControlPawn::BeginPlay()
{
	Super::BeginPlay();

	MainCanvasWidget = CreateWidget<UMainCanvasWidget>(GetWorld(), UMainCanvasWidgetClass);
	if (IsValid(MainCanvasWidget))
	{
		MainCanvasWidget->AddToViewport();
	}


	//TArray<AActor*> units;
	//UGameplayStatics::GetAllActorsOfClass(GetWorld(), AUnit::StaticClass(), units);

	//for (auto i : units)
	//{
	//	auto unit = Cast<AUnit>(i);

	//	TODO_Marker::TODO();
	//	unit->GetUnitActionComponent(EUnitActionType::Attack)
	//		->OnActionCompleted.AddDynamic(this, &AUnitControlPawn::OnUnitActionCompleted);
	//	unit->GetUnitActionComponent(EUnitActionType::Interact)
	//		->OnActionCompleted.AddDynamic(this, &AUnitControlPawn::OnUnitActionCompleted);
	//	unit->GetUnitActionComponent(EUnitActionType::Move)
	//	->OnActionCompleted.AddDynamic(this, &AUnitControlPawn::OnUnitActionCompleted);
	//	unit->GetUnitActionComponent(EUnitActionType::Wait)
	//		->OnActionCompleted.AddDynamic(this, &AUnitControlPawn::OnUnitActionCompleted);

	//}



}

//void AUnitControlPawn::PossessedBy(AController* NewController)
//{
//	Super::PossessedBy(NewController);
//	//InitGridPosition(StartGrid);
//}

// Called every frame
void AUnitControlPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//ControlPawn의 Grid위치에 맞게 보간이동함.

	AGridManager* gridManager = AGridManager::GetGridManager();
	if (IsValid(gridManager))
	{
		FVector worldLoc = gridManager->GridToWorld(PivotGrid);
		FVector currentLoc = GetActorLocation();
		FVector loc = FMath::VInterpTo(currentLoc, worldLoc, DeltaTime, 10.0f);

		float dist = FVector::Distance(currentLoc, loc);

		
		if (!FMath::IsNearlyEqual(dist, 0))
		{
			SetActorLocation(loc);
		}
	}
}

// Called to bind functionality to input
void AUnitControlPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//Applied EnhancedInput.

	// Get the player controller
	APlayerController* playerController = Cast<APlayerController>(GetController());

	if (!IsValid(playerController))
	{
		return;
	}

	//// Get the local player subsystem
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(playerController->GetLocalPlayer());
	
	// Clear out existing mapping, and add our mapping
	Subsystem->ClearAllMappings();
	Subsystem->AddMappingContext(InputMapping, 0);

	UEnhancedInputComponent* enhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!IsValid(enhancedInput))
	{
		return;
	}

	enhancedInput->BindAction(InputGridMove, ETriggerEvent::Triggered, this, FName("GridMove"));
	enhancedInput->BindAction(InputGridMove, ETriggerEvent::Started, this, FName("GridMoveStart"));
	enhancedInput->BindAction(InputGridMove, ETriggerEvent::Completed, this, FName("GridMoveEnd"));
	enhancedInput->BindAction(ControlEnter, ETriggerEvent::Completed, this, FName("HandleControlEnter"));
}

void AUnitControlPawn::InitGridPosition(const FGrid& Grid)
{
	PivotGrid = Grid;

	AGridManager* gridManager = AGridManager::GetGridManager();
	if (IsValid(gridManager))
	{
		FVector worldLoc = gridManager->GridToWorld(PivotGrid);
		SetActorLocation(worldLoc);
	}
	else
	{
		Debug::Print(DEBUG_TEXT("gridManager is Invalid."));
	}
}

void AUnitControlPawn::GridMove(const FInputActionValue& Val)
{
	//UI 모드나, Busy 상태면 이동 못함.
	if (PawnMode == EPawnMode::UI) //|| PawnMode == EPawnMode::Busy)
	{
		return;
	}

	//키보드 입력.. 이동할 방향임.
	const FVector2D moveVal = Val.Get<FVector2D>();

	//UE_LOG(LogTemp, Warning, TEXT("%f / %f"), moveVal.X, moveVal.Y);

	//이동버튼 누르는 동안...
	//0.15초마다 한칸씩 이동할 수 있게 버튼 누른 시간 누적합
	if (moveVal.Size() > 0.0f)
	{
		if (!GetWorld())
		{
			return;
		}
		MoveAccumulate += GetWorld()->GetDeltaSeconds();
	}

	if (MoveAccumulate >= MoveInterval)
	{
		FGrid move(0, 0);


		/*누른 버튼의 방향에 맞게 이동방향 Grid를 만듬*/
		if (moveVal.X != 0.0f)
		{
			if (moveVal.X >= 0.01f)
			{
				move.X = 1;
			}
			else if (moveVal.X < -0.01f)
			{
				move.X = -1;
			}
		}

		if (moveVal.Y != 0.0f)
		{
			if (moveVal.Y > 0.01f)
			{
				move.Y = 1;
			}
			else if (moveVal.Y < -0.01f)
			{
				move.Y = -1;
			}
		}

		//UE_LOG(LogTemp, Warning, TEXT("move grid : %d / %d"), move.X, move.Y);

		if (move.Size() > 0)
		{
			PivotGrid += move;
			MoveAccumulate = 0.0f;
		}
	}
}

void AUnitControlPawn::GridMoveStart(const FInputActionValue& Val)
{
	//최초 이동을 할땐 바로 이동할 수 있어야함.
	MoveAccumulate = 1.0f;
}

void AUnitControlPawn::GridMoveEnd(const FInputActionValue& Val)
{
	MoveAccumulate = 0.0f;
}

void AUnitControlPawn::HandleControlEnter(const bool& Val)
{
	/*if (PawnMode == EPawnMode::Busy)
	{
		return;
	}*/

	//Pawn의 현재 상태에 따라 Enter 버튼의 행동이 다름.
	//Selection : Pawn이 현재 Grid 위치의 유닛을 선택하는 모드
	//Action : 선택된 유닛의 Action을 실행하는 모드
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

bool AUnitControlPawn::TryUnitSelect()
{
	//Unit 선택을 시도함. MyUnit 태그가 붙어있으면 유닛을 선택할 것임.
	//결과에 따라 true or false return한다.

	AGridManager* gridManager = AGridManager::GetGridManager();

	if (!IsValid(gridManager))
	{
		Debug::Print(DEBUG_TEXT("gridManager is Invalid."));
		return false;
	}

	if (!gridManager->IsValidGrid(PivotGrid))
	{
		Debug::Print(DEBUG_TEXT("Grid is Invalid."));
		return false;
	}

	AUnit* unitOnGrid = gridManager->GetUnitAtGrid(PivotGrid);

	if (!IsValid(unitOnGrid))
	{
		Debug::Print(DEBUG_TEXT("No Unit."));
		return false;
	}

	if (!(unitOnGrid->IsThisUnitCanAction()))
	{
		Debug::Print(DEBUG_TEXT("this Unit Can't Do AnyThing."));
		return false;
	}

	if (unitOnGrid->GetTeamType() == ETeamType::Team01)
	{
		Debug::Print(DEBUG_TEXT("Unit Set!"));
		SetSelectedUnit(unitOnGrid);
		return true;
	}

	return false;
}

void AUnitControlPawn::SetSelectedUnit(AUnit* UnitToSelect)
{
	//Pawn이 현재 유닛을 선택한다.

	if (!IsValid(UnitToSelect)
		|| SelectedUnit == UnitToSelect) // 올바르지 않은 유닛이거나, 이미 선택한 유닛이면 더이상 하지 않음.
	{
		return;
	}

	SelectedUnit = UnitToSelect;

	if (OnSelectedUnitChanged.IsBound()) // 유닛이 변경됐다는 것을 알림.
	{
		OnSelectedUnitChanged.Broadcast();
	}

}

void AUnitControlPawn::SetSelectedAction(UUnitActionComponent* UnitActionToSelect)
{
	//Pawn이 실행하고 싶은 Unit의 Action을 선택함.

	if (SelectedAction == UnitActionToSelect)
	{
		Debug::Print(DEBUG_TEXT("InputAction is Equal with Currently Selected Action."));
		return;
	}

	SelectedAction = UnitActionToSelect;
	SelectedAction->SelectThisAction();

	if (OnSelectedActionChanged.IsBound())
	{
		OnSelectedActionChanged.Broadcast();
	}
}

void AUnitControlPawn::DoSelection()
{
	bool unitSelect = TryUnitSelect();

	if (unitSelect)
	{
		if (!IsValid(SelectedUnit))
		{
			Debug::Print(DEBUG_TEXT("Selected Unit is Invalid."));
			return;
		}

		if (IsValid(UnitActionsWidget))
		{
			UnitActionsWidget->RemoveFromParent();
			UnitActionsWidget = nullptr;
		}

		FVector2D screenPos;
		FVector unitLocation = SelectedUnit->GetActorLocation();
		APlayerController* playerController = Cast<APlayerController>(GetController());
		UGameplayStatics::ProjectWorldToScreen(
			playerController,
			unitLocation,
			screenPos,
			true);

		UnitActionsWidget = CreateWidget<UUnitActionListWidget>(GetWorld(), UnitActionListWidgetClass);
		MainCanvasWidget->MainCanvas->AddChild(UnitActionsWidget);

		UCanvasPanelSlot* canvasSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(UnitActionsWidget);
		if (IsValid(canvasSlot))
		{
			canvasSlot->SetPosition(screenPos);
			canvasSlot->SetSize(FVector2D(300, 500));
		}

		SetControlPawnMode(EPawnMode::UI);
		UnitActionsWidget->InitUnitActionsWidget(SelectedUnit);
	}
}

void AUnitControlPawn::DoDeselection()
{
	SelectedUnit = nullptr;
	SelectedAction = nullptr;
}

void AUnitControlPawn::DoAction()
{
	if (!IsValid(SelectedUnit))
	{
		Debug::Print(DEBUG_TEXT("Selected Unit is Invalid."));
		return;
	}

	if (!IsValid(SelectedAction))
	{
		Debug::Print(DEBUG_TEXT("Selected Action is Invalid."));
		return;
	}

	AGridManager* gridManager = AGridManager::GetGridManager();
	if (!IsValid(gridManager))
	{
		Debug::Print(DEBUG_TEXT("gridManager is Invalid."));
		return;
	}

	if (!gridManager->IsValidGrid(PivotGrid))
	{
		return;
	}

	SelectedAction->DealWithGridBeforeAction(PivotGrid);

}

void AUnitControlPawn::SetControlPawnMode(EPawnMode ModeInput)
{
	PawnMode = ModeInput;

	if (APlayerController* pc = Cast<APlayerController>(GetController()))
	{
		switch (ModeInput)
		{
		case EPawnMode::Selection:
			pc->SetInputMode(FInputModeGameOnly());
			break;
		case EPawnMode::Action:
			pc->SetInputMode(FInputModeGameOnly());
			break;
		case EPawnMode::UI:
			pc->SetInputMode(FInputModeUIOnly());
			break;
		case EPawnMode::Busy:
			pc->SetInputMode(FInputModeGameOnly());
			break;
		default:
			pc->SetInputMode(FInputModeGameOnly());
			break;
		}
	}

}

void AUnitControlPawn::SetBusyOrNot(bool BusyInput)
{
	PawnMode = BusyInput ? EPawnMode::Busy : EPawnMode::Selection;
}

void AUnitControlPawn::OnUnitActionCompleted()
{
	OnUnitActionCompleted_virtual();
}

void AUnitControlPawn::OnUnitActionCompleted_virtual()
{
	SetControlPawnMode(EPawnMode::Selection);
	SetBusyOrNot(false);
	DoDeselection();

	ATurnManager* turnManager = ATurnManager::GetTurnManager();
	bool bcheck = turnManager->CheckCurrentTurnValidation();
	if (!bcheck)
	{
		turnManager->NextTurn();
	}

	Debug::Print(DEBUG_TEXT("OnUnitActionCompleted"));
}

void AUnitControlPawn::FindAllManagingUnits()
{

	TArray<AActor*> outUnit;
	UGameplayStatics::GetAllActorsOfClass(this, AUnit::StaticClass(), outUnit);

	ETeamType teamCompare = ETeamType::Team01;

	switch (PawnTurnType)
	{
	case ETurnType::Team01Turn:
		teamCompare = ETeamType::Team01;
		break;
	case ETurnType::Team02Turn:
		teamCompare = ETeamType::Team02;
		break;
	case ETurnType::Team03Turn:
		teamCompare = ETeamType::Team03;
		break;
	default:
		break;
	}

	for (auto actor : outUnit)
	{
		AUnit* unit = Cast<AUnit>(actor);

		if (!IsValid(unit))
		{
			continue;
		}

		if (unit->GetTeamType() != teamCompare)
		{
			continue;
		}

		TArray<UActorComponent*> unitActions;
		unit->GetComponents(UUnitActionComponent::StaticClass(), unitActions);

		for (UActorComponent* unitAction : unitActions)
		{
			UUnitActionComponent* unitAction_Cast =
				Cast<UUnitActionComponent>(unitAction);

			if (!IsValid(unitAction_Cast))
			{
				continue;
			}

			//unitAction_Cast->OnActionCompleteForControlPawn.Clear();
			unitAction_Cast->OnActionEnd.AddDynamic(this, &AUnitControlPawn::OnUnitActionCompleted);
			Debug::Print(DEBUG_TEXT("UnitAction Bind to Control Pawn OK"));
		}

	}

}

