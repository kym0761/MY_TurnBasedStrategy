// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitAttackActionComponent.h"
#include "UnitCharacter.h"
#include "GridManager.h"


UUnitAttackActionComponent::UUnitAttackActionComponent()
{
    MaxActionRange = 1;
}

FString UUnitAttackActionComponent::GetActionName() const
{
    return FString("Attack");
}

TArray<FGrid> UUnitAttackActionComponent::GetValidActionGridArray() const
{
	TArray<FGrid> validArray;

	FGrid unitGrid = Unit->GetGrid();

	AGridManager* gridManager = AGridManager::GetGridManager();

	if (!IsValid(gridManager))
	{
		UE_LOG(LogTemp, Warning, TEXT("Grid Manager not Valid"));
		return validArray;
	}

	for (int x = -MaxActionRange; x <= MaxActionRange; x++)
	{
		for (int y = -MaxActionRange; y <= MaxActionRange; y++)
		{
			FGrid offsetGrid = FGrid(x, y);

			FGrid resultGrid = unitGrid + offsetGrid;

			if (FMath::Abs(x) + FMath::Abs(y) > MaxActionRange)
			{
				continue;
			}

			//존재하지 않는 Grid
			if (!gridManager->IsValidGrid(resultGrid))
			{
				continue;
			}

			////지금 현재 Unit의 위치
			if (resultGrid == unitGrid)
			{
				continue;
			}

			//상대가 같은 팀 tag가 붙어있으면 스킵.
			AUnitCharacter* targetUnit = gridManager->GetUnitAtGrid(resultGrid);
			if (IsValid(targetUnit) && GetOwner()->Tags.Num() > 0 && targetUnit->ActorHasTag(GetOwner()->Tags[0]))
			{
				continue;
			}

			//통과하면 문제없으니 validArray에 추가
			validArray.Add(resultGrid);
		}
	}



	return validArray;
}

TArray<FGridVisualData> UUnitAttackActionComponent::GetValidActionGridVisualDataArray() const
{
	TArray<FGridVisualData> validVisualDataArray;
	FGrid unitGrid = Unit->GetGrid();

	AGridManager* gridManager = AGridManager::GetGridManager();

	if (!IsValid(gridManager))
	{
		UE_LOG(LogTemp, Warning, TEXT("Grid Manager not Valid"));
		return validVisualDataArray;
	}


	for (int x = -MaxActionRange; x <= MaxActionRange; x++)
	{
		for (int y = -MaxActionRange; y <= MaxActionRange; y++)
		{
			if (FMath::Abs(x) + FMath::Abs(y) > MaxActionRange)
			{
				continue;
			}

			FGrid offsetGrid = FGrid(x, y);
			FGrid resultGrid = unitGrid + offsetGrid;

			//존재하지 않는 Grid
			if (!gridManager->IsValidGrid(resultGrid))
			{
				continue;
			}

			FGridVisualData resultData;
			resultData.Grid = resultGrid;
			resultData.GridVisualType = EGridVisualType::NO;

			////지금 현재 Unit의 위치
			if (resultGrid == unitGrid)
			{
				continue;
			}

			////누군가 점유중
			//if (gridManager->HasAnyUnitOnGrid(resultGrid))
			//{
			//	resultData.GridVisualType = EGridVisualType::NO;
			//}

			//상대가 같은 팀 tag가 붙어있으면 스킵.
			AUnitCharacter* targetUnit = gridManager->GetUnitAtGrid(resultGrid);
			if (IsValid(targetUnit) && GetOwner()->Tags.Num() > 0 && targetUnit->ActorHasTag(GetOwner()->Tags[0]))
			{
				continue;
			}

			//통과하면 문제없으니 validArray에 추가

			validVisualDataArray.Add(resultData);
		}
	}


	return validVisualDataArray;



}

void UUnitAttackActionComponent::TakeAction(FGrid Grid)
{
	UE_LOG(LogTemp, Warning, TEXT("Attack At : %s"), *Grid.ToString());
}
