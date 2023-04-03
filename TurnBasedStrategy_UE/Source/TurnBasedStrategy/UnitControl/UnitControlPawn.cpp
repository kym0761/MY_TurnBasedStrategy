// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitControlPawn.h"

#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"

//#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"

#include "Manager/GridManager.h"
#include "UnitCore/UnitCharacter.h"

#include "UMG/UnitActionListWidget.h"
#include "UMG/MainCanvasWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/CanvasPanel.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "UnitAction/UnitActionComponent.h"


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
	
	InitGridPosition(StartGrid);

	MainCanvasWidget = CreateWidget<UMainCanvasWidget>(GetWorld(), UMainCanvasWidgetClass);
	if (IsValid(MainCanvasWidget))
	{
		MainCanvasWidget->AddToViewport();
	}

}

// Called every frame
void AUnitControlPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//ControlPawn�� Grid��ġ�� �°� �����̵���.
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
}

void AUnitControlPawn::GridMove(const FInputActionValue& Val)
{
	//UI ��峪, Busy ���¸� �̵� ����.
	if (PawnMode == EPawnMode::UI || PawnMode == EPawnMode::Busy)
	{
		return;
	}

	//Ű���� �Է�.. �̵��� ������.
	const FVector2D moveVal = Val.Get<FVector2D>();

	//UE_LOG(LogTemp, Warning, TEXT("%f / %f"), moveVal.X, moveVal.Y);

	//�̵���ư ������ ����...
	//���� 0.15�ʸ��� ��ĭ�� �̵��� �� �ְ� ��ư ���� �ð� ������
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


		/*���� ��ư�� ���⿡ �°� �̵����� Grid�� ����*/
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
	//���� �̵��� �Ҷ� �ٷ� �̵��� �� �־����.
	MoveAccumulate = 1.0f;
}

void AUnitControlPawn::GridMoveEnd(const FInputActionValue& Val)
{
	MoveAccumulate = 0.0f;
}

void AUnitControlPawn::HandleControlEnter(const bool& Val)
{
	if (bIsBusy)
	{
		return;
	}

	//Pawn�� ���� ���¿� ���� Enter ��ư�� �ൿ�� �ٸ�.
	//Selection : Pawn�� ���� Grid ��ġ�� ������ �����ϴ� ���
	//Action : ���õ� ������ Action�� �����ϴ� ���
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
	//Unit ������ �õ���. MyUnit �±װ� �پ������� ������ ������ ����.
	//����� ���� true or false return�Ѵ�.

	AGridManager* gridManager = AGridManager::GetGridManager();
	if (!IsValid(gridManager))
	{
		UE_LOG(LogTemp, Warning, TEXT("GridManager Not Valid"));
		return false;
	}

	if (!gridManager->IsValidGrid(PivotGrid))
	{
		UE_LOG(LogTemp, Warning, TEXT("Grid is Not Valid"));
		return false;
	}

	auto unitOnGrid = gridManager->GetUnitAtGrid(PivotGrid);

	if (!IsValid(unitOnGrid))
	{
		UE_LOG(LogTemp, Warning, TEXT("No Unit."));
		return false;
	}

	if (!(unitOnGrid->IsThisUnitCanAction()))
	{
		UE_LOG(LogTemp, Warning, TEXT("this Unit Can't Do AnyThing."));
		return false;
	}

	if (unitOnGrid->ActorHasTag(FName("MyUnit")))
	{
		UE_LOG(LogTemp, Warning, TEXT("Unit Set!"));
		SetSelectedUnit(unitOnGrid);
		return true;
	}

	return false;
}

void AUnitControlPawn::SetSelectedUnit(AUnitCharacter* InputUnit)
{
	//Pawn�� ���� ������ �����Ѵ�.

	if (!IsValid(InputUnit)
		|| SelectedUnit == InputUnit) // �ùٸ��� ���� �����̰ų�, �̹� ������ �����̸� ���̻� ���� ����.
	{
		return;
	}

	SelectedUnit = InputUnit;

	if (OnSelectedUnitChanged.IsBound()) // ������ ����ƴٴ� ���� �˸�.
	{
		OnSelectedUnitChanged.Broadcast();
	}

}

void AUnitControlPawn::SetSelectedAction(UUnitActionComponent* InputUnitAction)
{
	//Pawn�� �����ϰ� ���� Unit�� Action�� ������.

	if (SelectedAction == InputUnitAction)
	{
		UE_LOG(LogTemp, Warning, TEXT(" InputAction is Equal with Currently Selected Action. -- AUnitControlPawn::SetSelectedAction()"));
		return;
	}

	SelectedAction = InputUnitAction;
	SelectedAction->SelectThisAction();

	if (OnSelectedActionChanged.IsBound())
	{
		OnSelectedActionChanged.Broadcast();
	}
}

void AUnitControlPawn::DoSelection()
{
	//������ ������� ����
	//�� Function�� ���콺 Ŭ�� ��ġ�� �ִ� ������ �����ϱ� ���� ������.
	//���� Pawn�� WASD�� Grid ���� �����̰�
	//���� Grid�� ������ �����ϴ� ������ �����. 

	bool unitSelect = TryUnitSelect();

	if (unitSelect)
	{
		if (!IsValid(SelectedUnit))
		{
			UE_LOG(LogTemp, Warning, TEXT("Selected Unit Not Valid"));
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
		UE_LOG(LogTemp, Warning, TEXT("false.. Selected Unit Not Valid"));
		return;
	}

	if (!IsValid(SelectedAction))
	{
		UE_LOG(LogTemp, Warning, TEXT("false.. Selected Action Not Valid"));
		return;
	}

	AGridManager* gridManager = AGridManager::GetGridManager();
	if (!IsValid(gridManager))
	{
		UE_LOG(LogTemp, Warning, TEXT("Action Failed Cause GridManager Can't be found"));
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
}

void AUnitControlPawn::SetBusyOrNot(bool BusyInput)
{
	bIsBusy = BusyInput;
}

void AUnitControlPawn::OnUnitActionCompleted()
{
	SetControlPawnMode(EPawnMode::Selection);
	SetBusyOrNot(false);
	DoDeselection();
	UE_LOG(LogTemp, Warning, TEXT("AUnitControlPawn::OnUnitActionCompleted()"));
}

void AUnitControlPawn::FindAllPlayerUnits()
{
	AGridManager* gridManager = AGridManager::GetGridManager();

	if (!IsValid(gridManager))
	{
		UE_LOG(LogTemp, Warning, TEXT("Grid Manager is not Valid"));
		return;
	}

	auto unitArr = gridManager->GetAllUnitInGridSystem();
	TArray<AUnitCharacter*> playerArr;

	for (auto unit : unitArr)
	{
		if (!IsValid(unit))
		{
			continue;
		}

		if (unit->ActorHasTag(TEXT("MyUnit")))
		{
			playerArr.Add(unit);
		}
	}

	for (auto unit : playerArr)
	{
		if (!IsValid(unit))
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

			unitAction_Cast->OnActionCompleteForControlPawn.Clear();
			unitAction_Cast->OnActionCompleteForControlPawn.AddDynamic(this, &AUnitControlPawn::OnUnitActionCompleted);
			UE_LOG(LogTemp, Warning, TEXT("AUnitControlPawn::FindAllPlayerUnits() Bind OK"));
		}
	}
}

