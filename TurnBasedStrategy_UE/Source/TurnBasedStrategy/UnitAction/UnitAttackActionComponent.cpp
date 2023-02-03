// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitAttackActionComponent.h"
#include "../UnitCharacter.h"
#include "../Grid/GridManager.h"


UUnitAttackActionComponent::UUnitAttackActionComponent()
{
    MaxActionRange = 1;

	ActionName = FString("Attack");
}

TArray<FGrid> UUnitAttackActionComponent::GetValidActionGridArray() const
{
	TArray<FGrid> validArray;

	FGrid unitGrid = Unit->GetGrid();

	AGridManager* gridManager = AGridManager::GetGridManager();

	if (!IsValid(gridManager))
	{
		UE_LOG(LogTemp, Warning, TEXT("Grid Manager is not Valid"));
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

			//�������� �ʴ� Grid
			if (!gridManager->IsValidGrid(resultGrid))
			{
				continue;
			}

			////���� ���� Unit�� ��ġ
			if (resultGrid == unitGrid)
			{
				continue;
			}

			//��밡 ���� �� tag�� �پ������� ��ŵ.
			AUnitCharacter* targetUnit = gridManager->GetUnitAtGrid(resultGrid);
			if (IsValid(targetUnit) && GetOwner()->Tags.Num() > 0 && targetUnit->ActorHasTag(GetOwner()->Tags[0]))
			{
				continue;
			}

			//����ϸ� ���������� validArray�� �߰�
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
		UE_LOG(LogTemp, Warning, TEXT("Grid Manager is not Valid"));
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

			//�������� �ʴ� Grid
			if (!gridManager->IsValidGrid(resultGrid))
			{
				continue;
			}

			FGridVisualData resultData;
			resultData.Grid = resultGrid;
			resultData.GridVisualType = EGridVisualType::NO;

			////���� ���� Unit�� ��ġ
			if (resultGrid == unitGrid)
			{
				continue;
			}

			//��밡 ���� �� tag�� �پ������� ��ŵ.
			AUnitCharacter* targetUnit = gridManager->GetUnitAtGrid(resultGrid);
			if (IsValid(targetUnit) && GetOwner()->Tags.Num() > 0 && targetUnit->ActorHasTag(GetOwner()->Tags[0]))
			{
				continue;
			}

			//����ϸ� ���������� validArray�� �߰�

			validVisualDataArray.Add(resultData);
		}
	}


	return validVisualDataArray;



}

void UUnitAttackActionComponent::TakeAction(FGrid Grid)
{
	TArray<FGrid> tempArr = GetValidActionGridArray();
	

	//TODO : ������ �ִ� Animation�� Play & ������ ó��

	if (tempArr.Contains(Grid))
	{
		UE_LOG(LogTemp, Warning, TEXT("Attack At : %s"), *Grid.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("You Selected which is Not Valid Grid. ---> Grid Pos : %s"), *Grid.ToString());
	}

	if (OnActionEnd.IsBound())
	{
		OnActionEnd.Broadcast();
	}

	
}
