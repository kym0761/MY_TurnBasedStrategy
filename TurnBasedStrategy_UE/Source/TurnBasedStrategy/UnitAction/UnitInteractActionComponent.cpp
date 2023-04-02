// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitInteractActionComponent.h"

#include "UnitCore/UnitCharacter.h"
#include "Manager/GridManager.h"


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
	TSet<FGrid> validSet;

	FGrid unitGrid = Unit->GetGrid();
	AGridManager* gridManager = AGridManager::GetGridManager();


	if (!IsValid(gridManager))
	{
		UE_LOG(LogTemp, Warning, TEXT("Grid Manager is not Valid"));
		return validSet;
	}

	TArray<int32> dx{ 1,-1,0,0 };
	TArray<int32> dy{ 0,0,-1,1 };

	for (int i = 0; i < dx.Num(); i++)
	{
		FGrid offsetGrid = FGrid(dx[i], dy[i]);
		FGrid resultGrid = unitGrid + offsetGrid;

		//�������� �ʴ� Grid
		if (!gridManager->IsValidGrid(resultGrid))
		{
			continue;
		}


		AUnitCharacter* targetUnit = gridManager->GetUnitAtGrid(resultGrid);
		if (!IsValid(targetUnit) || GetOwner()->Tags.Num() > 0 && !targetUnit->ActorHasTag(GetOwner()->Tags[0]))
		{
			continue;
		}

		//����ϸ� ���������� validSet�� �߰�
		validSet.Add(resultGrid);
	}

	return validSet;
}

TSet<FGridVisualData> UUnitInteractActionComponent::GetValidActionGridVisualDataSet() const
{
	TSet<FGridVisualData> validVisualDataSet;
	FGrid unitGrid = Unit->GetGrid();

	AGridManager* gridManager = AGridManager::GetGridManager();

	if (!IsValid(gridManager))
	{
		UE_LOG(LogTemp, Warning, TEXT("Grid Manager is not Valid"));
		return validVisualDataSet;
	}

	TArray<int32> dx{ 1,-1,0,0 };
	TArray<int32> dy{ 0,0,-1,1 };

	for (int i = 0; i < dx.Num(); i++)
	{
		FGrid resultGrid = FGrid(dx[i], dy[i]);
		resultGrid += unitGrid;

		//�������� �ʴ� Grid
		if (!gridManager->IsValidGrid(resultGrid))
		{
			continue;
		}

		AUnitCharacter* targetUnit = gridManager->GetUnitAtGrid(resultGrid);
		if (!IsValid(targetUnit) || GetOwner()->Tags.Num() > 0 && !targetUnit->ActorHasTag(GetOwner()->Tags[0]))
		{
			continue;
		}

		FGridVisualData resultVisualData;
		resultVisualData.Grid = resultGrid;
		resultVisualData.GridVisualType = EGridVisualType::Warning;

		//����ϸ� ���������� validSet�� �߰�
		validVisualDataSet.Add(resultVisualData);
	}


	return validVisualDataSet;
}

void UUnitInteractActionComponent::TakeAction(const FGrid& Grid)
{
	TSet<FGrid> tempArr = GetValidActionGridSet();

	if (tempArr.Contains(Grid))
	{
		UE_LOG(LogTemp, Warning, TEXT("InterAct At : %s"), *Grid.ToString());
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
		UE_LOG(LogTemp, Warning, TEXT("Grid Manager is not Valid"));
		return;
	}

	gridManager->RemoveAllGridVisual();

	Super::ActionEnd();
}

void UUnitInteractActionComponent::ActionSelected()
{
	Super::ActionSelected();

}
