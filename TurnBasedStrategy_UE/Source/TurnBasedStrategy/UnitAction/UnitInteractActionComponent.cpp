// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitInteractActionComponent.h"

#include "../UnitCharacter.h"
#include "../Manager/GridManager.h"


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

TArray<FGrid> UUnitInteractActionComponent::GetValidActionGridArray() const
{
	TArray<FGrid> validArray;

	FGrid unitGrid = Unit->GetGrid();
	AGridManager* gridManager = AGridManager::GetGridManager();


	if (!IsValid(gridManager))
	{
		UE_LOG(LogTemp, Warning, TEXT("Grid Manager is not Valid"));
		return validArray;
	}

	TArray<int32> dx{ 1,-1,0,0 };
	TArray<int32> dy{ 0,0,-1,1 };

	for (int i = 0; i < dx.Num(); i++)
	{
		FGrid offsetGrid = FGrid(dx[i], dy[i]);
		FGrid resultGrid = unitGrid + offsetGrid;

		//존재하지 않는 Grid
		if (!gridManager->IsValidGrid(resultGrid))
		{
			continue;
		}


		AUnitCharacter* targetUnit = gridManager->GetUnitAtGrid(resultGrid);
		if (!IsValid(targetUnit) || GetOwner()->Tags.Num() > 0 && !targetUnit->ActorHasTag(GetOwner()->Tags[0]))
		{
			continue;
		}

		//통과하면 문제없으니 validArray에 추가
		validArray.Add(resultGrid);
	}

	return validArray;
}

TArray<FGridVisualData> UUnitInteractActionComponent::GetValidActionGridVisualDataArray() const
{
	TArray<FGridVisualData> validVisualDataArray;
	FGrid unitGrid = Unit->GetGrid();

	AGridManager* gridManager = AGridManager::GetGridManager();

	if (!IsValid(gridManager))
	{
		UE_LOG(LogTemp, Warning, TEXT("Grid Manager is not Valid"));
		return validVisualDataArray;
	}

	TArray<int32> dx{ 1,-1,0,0 };
	TArray<int32> dy{ 0,0,-1,1 };

	for (int i = 0; i < dx.Num(); i++)
	{
		FGrid resultGrid = FGrid(dx[i], dy[i]);
		resultGrid += unitGrid;

		//존재하지 않는 Grid
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

		//통과하면 문제없으니 validArray에 추가
		validVisualDataArray.Add(resultVisualData);
	}


	return validVisualDataArray;
}

void UUnitInteractActionComponent::TakeAction(const FGrid& Grid)
{
	TArray<FGrid> tempArr = GetValidActionGridArray();

	if (tempArr.Contains(Grid))
	{
		UE_LOG(LogTemp, Warning, TEXT("InterAct At : %s"), *Grid.ToString());
	}

	if (OnActionEnd.IsBound())
	{
		OnActionEnd.Broadcast();
	}


}

void UUnitInteractActionComponent::OnActionStartFunc()
{
	Super::OnActionStartFunc();
}

void UUnitInteractActionComponent::OnActionEndFunc()
{
	Super::OnActionEndFunc();

	AGridManager* gridManager = AGridManager::GetGridManager();

	if (!IsValid(gridManager))
	{
		UE_LOG(LogTemp, Warning, TEXT("Grid Manager is not Valid"));
		return;
	}

	gridManager->RemoveAllGridVisual();

}

void UUnitInteractActionComponent::OnActionSelectedFunc()
{
	Super::OnActionSelectedFunc();

}
