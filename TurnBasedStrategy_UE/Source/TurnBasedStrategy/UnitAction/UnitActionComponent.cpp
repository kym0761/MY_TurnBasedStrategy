// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitActionComponent.h"
#include "UnitCore/UnitCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "UnitControl/UnitControlPawn.h"


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

}


// Called when the game starts
void UUnitActionComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

	Unit = Cast<AUnitCharacter>(GetOwner());
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

void UUnitActionComponent::DealWithGridBeforeAction(const FGrid& Grid)
{
	UE_LOG(LogTemp, Warning, TEXT(" :) "));
}

void UUnitActionComponent::TakeAction(const FGrid& Grid)
{
	//Do Nothing?

	ActionEnd();
}

bool UUnitActionComponent::IsValidActionGrid(const FGrid& Grid) const
{
	return GetValidActionGridArray().Contains(Grid);
}

TArray<FGridVisualData> UUnitActionComponent::GetValidActionGridVisualDataArray() const
{
	return TArray<FGridVisualData>();
}

TArray<FGrid> UUnitActionComponent::GetValidActionGridArray() const
{
	return TArray<FGrid>();
}

AUnitCharacter* UUnitActionComponent::GetUnit() const
{
	return Unit;
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


	return GetValidActionGridArray().Num() != 0;

}

void UUnitActionComponent::SetCanDoActionThisTurn(bool InputBool)
{
	bCanDoActionThisTurn = InputBool;
}

void UUnitActionComponent::ActionStart()
{
	APlayerController* playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (IsValid(playerController))
	{
		AUnitControlPawn* pawn = Cast<AUnitControlPawn>(playerController->GetPawn());
	
		if (IsValid(pawn))
		{
			pawn->SetControlPawnMode(EPawnMode::Busy);
			pawn->SetBusyOrNot(true);
		}
	}

	if (OnActionStart.IsBound())
	{
		OnActionStart.Broadcast();
	}
}

void UUnitActionComponent::ActionEnd()
{
	APlayerController* playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	
	if (IsValid(playerController))
	{
		AUnitControlPawn* pawn = Cast<AUnitControlPawn>(playerController->GetPawn());

		if (IsValid(pawn))
		{
			pawn->SetControlPawnMode(EPawnMode::Selection);
			pawn->SetBusyOrNot(false);
			pawn->DoDeselection();
			SetCanDoActionThisTurn(false);

			//UE_LOG(LogTemp, Warning, TEXT("UUnitActionComponent::ActionEnd(), Setting Pawn."));
		}
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

void UUnitActionComponent::TestFunction()
{
	TakeAction(ThinkAIBestActionGrid());
}

void UUnitActionComponent::TestUnitAction()
{
	TestFunction();
}

void UUnitActionComponent::SelectThisAction()
{
	ActionSelected();
}

void UUnitActionComponent::BindToOnActionStart(FScriptDelegate ToBind)
{
	OnActionStart.AddUnique(ToBind);
}

void UUnitActionComponent::BindToOnActionEnd(FScriptDelegate ToBind)
{
	OnActionEnd.AddUnique(ToBind);
}

void UUnitActionComponent::BindToOnActionSelected(FScriptDelegate ToBind)
{
	OnActionSelected.AddUnique(ToBind);
}
