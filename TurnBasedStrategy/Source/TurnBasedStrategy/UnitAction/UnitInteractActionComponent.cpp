// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitInteractActionComponent.h"

#include "UnitCore/Unit.h"
#include "Manager/GridManager.h"
#include "Manager/SRPG_GameMode.h"

#include "DebugHelper.h"

UUnitInteractActionComponent::UUnitInteractActionComponent()
{
	MaxActionRange = 1;

	ActionName = FString("Interact");
}

void UUnitInteractActionComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UUnitInteractActionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

TSet<FGrid> UUnitInteractActionComponent::GetValidActionGridSet() const
{
	//Interact는 현재 위치에서 상하좌우에서만 일어날 것임.

	TSet<FGrid> validSet;

	AUnit* unit = GetOwningUnit();
	if (!IsValid(unit))
	{
		return validSet;
	}

	FGrid unitGrid = unit->GetGrid();

	ASRPG_GameMode* gameMode = ASRPG_GameMode::GetSRPG_GameMode(GetWorld());
	if (!IsValid(gameMode))
	{
		Debug::Print(DEBUG_TEXT("Grid Manager is Invalid."));
		return validSet;
	}

	TArray<int32> dx{ 1,-1,0,0 };
	TArray<int32> dy{ 0,0,-1,1 };

	for (int i = 0; i < dx.Num(); i++)
	{
		FGrid offsetGrid = FGrid(dx[i], dy[i]);
		FGrid resultGrid = unitGrid + offsetGrid;

		//존재하지 않는 Grid
		if (!gameMode->IsValidGrid(resultGrid))
		{
			continue;
		}

		AUnit* targetUnit = gameMode->GetUnitAtGrid(resultGrid);
		if (!IsValid(targetUnit) || GetOwner()->Tags.Num() > 0 && !targetUnit->ActorHasTag(GetOwner()->Tags[0]))
		{
			continue;
		}

		//통과하면 문제없으니 validSet에 추가
		validSet.Add(resultGrid);
	}

	return validSet;
}

TSet<FGridVisualData> UUnitInteractActionComponent::GetValidActionGridVisualDataSet() const
{
	//위 Function의 GridVisualData 버전

	TSet<FGridVisualData> validVisualDataSet;
	auto grids = GetValidActionGridSet();

	for (auto grid : grids)
	{
		FGridVisualData resultVisualData;
		resultVisualData.Grid = grid;
		resultVisualData.GridVisualType = EGridVisualType::Warning;

		validVisualDataSet.Add(resultVisualData);
	}

	return validVisualDataSet;
}

void UUnitInteractActionComponent::TakeAction(const FGrid& Grid)
{
	//TODO : Interact가 되도록 만들어야함.

	TSet<FGrid> tempArr = GetValidActionGridSet();

	if (tempArr.Contains(Grid))
	{
		Debug::Print(DEBUG_TEXT("InterAct At : ") + Grid.ToString());
	}


	ActionEnd();

}

void UUnitInteractActionComponent::DealWithGridBeforeAction(const FGrid& Grid)
{
	TakeAction(Grid);
}

void UUnitInteractActionComponent::ActionStart()
{
	Super::ActionStart();
}

void UUnitInteractActionComponent::ActionEnd()
{
	AGridManager* gridManager = AGridManager::GetGridManager();

	if (!IsValid(gridManager))
	{
		Debug::Print(DEBUG_TEXT("Grid Manager is Invalid."));
		return;
	}

	gridManager->RemoveAllGridVisual();

	Super::ActionEnd();
}

void UUnitInteractActionComponent::ActionSelected()
{
	Super::ActionSelected();

}
