// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitActionComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UnitControl/UnitControlPawn.h"

#include "Manager/TurnManager.h"
#include "DebugHelper.h"
#include "UnitCore/Unit.h"

// Sets default values for this component's properties
UUnitActionComponent::UUnitActionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	MaxActionRange = 0;
	bCanDoActionThisTurn = true;

	ActionName = FString("BaseAction");

	bWantsInitializeComponent = true;

}


// Called when the game starts
void UUnitActionComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}

void UUnitActionComponent::InitializeComponent()
{
	Super::InitializeComponent();

	//TArray<AActor*> outActors;
	//UGameplayStatics::GetAllActorsOfClass(GetWorld(), AUnitControlPawn::StaticClass(), outActors);

	//for (auto i : outActors)
	//{
	//	AUnitControlPawn* unitControlpawn = CastChecked<AUnitControlPawn>(i);

	//	FScriptDelegate toBind;
	//	toBind.BindUFunction(unitControlpawn, FName(TEXT("OnUnitActionCompleted")));
	//	OnActionEnd.Add(toBind);
	//}

}


// Called every frame
void UUnitActionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

FString UUnitActionComponent::GetActionName() const
{
	return ActionName;
}

AUnit* UUnitActionComponent::GetOwningUnit() const
{
	return Cast<AUnit>(GetOwner());
}

void UUnitActionComponent::DealWithGridBeforeAction(const FGrid& Grid)
{
	UE_LOG(LogTemp, Warning, TEXT(" :) "));
}

void UUnitActionComponent::TakeAction(const FGrid& Grid)
{
	//기본 Action이므로 아무것도 안하고 끝남.

	ActionEnd();
}

bool UUnitActionComponent::IsValidActionGrid(const FGrid& Grid) const
{
	return GetValidActionGridSet().Contains(Grid);
}

TSet<FGridVisualData> UUnitActionComponent::GetValidActionGridVisualDataSet() const
{
	return TSet<FGridVisualData>();
}

TSet<FGrid> UUnitActionComponent::GetValidActionGridSet() const
{
	return TSet<FGrid>();
}

bool UUnitActionComponent::IsCanDoActionThisTurn() const
{
	return bCanDoActionThisTurn;
}

bool UUnitActionComponent::IsCurrentlyAvailableAction() const
{
	if (ActionName == "Wait" || ActionName == "Move")
	{
		return IsCanDoActionThisTurn();
	}


	return GetValidActionGridSet().Num() != 0;

}

void UUnitActionComponent::SetCanDoActionThisTurn(bool InputBool)
{
	bCanDoActionThisTurn = InputBool;
}

void UUnitActionComponent::ActionStart()
{
	//액션이 시작되면 IsBusy를 true로 해서 다음 행동이 멋대로 발동되는 것을 막는다.
	//ex) 마지막 유닛이 전투를 끝내고, 적 유닛이 아직 삭제되지 않은 상태에서 턴이 넘어가는 것.
	auto turnManager = ATurnManager::GetTurnManager();
	if (IsValid(turnManager))
	{
		turnManager->SetIsBusy(true);
	}

	//APlayerController* playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	//if (IsValid(playerController))
	//{
	//	AUnitControlPawn* pawn = Cast<AUnitControlPawn>(playerController->GetPawn());
	//
	//	if (IsValid(pawn))
	//	{
	//		pawn->SetControlPawnMode(EPawnMode::Busy);
	//		pawn->SetBusyOrNot(true);
	//	}
	//}

	if (OnActionStart.IsBound())
	{
		OnActionStart.Broadcast();
	}
}

void UUnitActionComponent::ActionEnd()
{
	Debug::Print(DEBUG_TEXT("Base::ActionEnd"));
	SetCanDoActionThisTurn(false);

	auto turnManager = ATurnManager::GetTurnManager();
	if (IsValid(turnManager))
	{
		turnManager->SetIsBusy(false);
	}

	if (OnActionEnd.IsBound())
	{
		OnActionEnd.Broadcast();
	}

}

void UUnitActionComponent::ActionSelected()
{
	APlayerController* playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	
	if (IsValid(playerController))
	{
		AUnitControlPawn* pawn = Cast<AUnitControlPawn>(playerController->GetPawn());

		if (IsValid(pawn))
		{
			pawn->SetControlPawnMode(EPawnMode::Action);
		}
	}

	if (OnActionSelected.IsBound())
	{
		OnActionSelected.Broadcast();
	}
}

FGrid UUnitActionComponent::ThinkAIBestActionGrid()
{
	return FGrid();
}

int32 UUnitActionComponent::CalculateActionValue(FGrid& CandidateGrid)
{
	return 0;
}

void UUnitActionComponent::AI_Action()
{
	TakeAction(ThinkAIBestActionGrid());
}

void UUnitActionComponent::SelectThisAction()
{
	ActionSelected();
}

void UUnitActionComponent::Execute_AI_Action()
{
	AI_Action();
}

