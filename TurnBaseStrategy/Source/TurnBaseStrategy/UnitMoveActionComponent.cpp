// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitMoveActionComponent.h"
#include "UnitCharacter.h"

#include "GridManager.h"
#include "Kismet/GameplayStatics.h"


UUnitMoveActionComponent::UUnitMoveActionComponent()
{
	MaxActionRange = 5;
}

FString UUnitMoveActionComponent::GetActionName() const
{
	return FString("Move");
}

TArray<FGrid> UUnitMoveActionComponent::GetValidActionGridArray() const
{
	TArray<FGrid> validArray;

	FGrid unitGrid = Unit->GetGrid();

	AGridManager* gridManager = Cast<AGridManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGridManager::StaticClass()));
	
	if (!IsValid(gridManager))
	{
		return validArray;
	}


	for (int x = -MaxActionRange; x <= MaxActionRange; x++)
	{
		for (int y = -MaxActionRange; y <= MaxActionRange; y++)
		{
			FGrid offsetGrid = FGrid(x, y);

			FGrid testGrid = unitGrid + offsetGrid;


			//존재하지 않는 Grid
			if (!gridManager->IsValidGrid(testGrid))
			{
				continue;
			}
			
			//지금 현재 Unit의 위치
			if (testGrid == unitGrid)
			{
				continue;
			}

			//누군가 점유중이면 Skip
			if (gridManager->HasAnyUnitOnGrid(testGrid))
			{
				continue;
			}

			//걸을 수 있는 위치?
			if (!gridManager->IsWalkableGrid(testGrid))
			{
				continue;
			}

			//도착 가능한 위치?
			if (!gridManager->HasPath(unitGrid, testGrid))
			{
				continue;
			}

			//의도와 달리 먼 거리?
			if (gridManager->GetPathLength(unitGrid, testGrid) > MaxActionRange)
			{
				continue;
			}

			//통과하면 문제없으니 validArray에 추가

			validArray.Add(testGrid);
		}
	}


	return TArray<FGrid>();
}

void UUnitMoveActionComponent::TakeAction(FGrid Grid)
{
	AGridManager* gridManager = Cast<AGridManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGridManager::StaticClass()));
	if (!IsValid(gridManager))
	{
		return;
	}

	int32 pathLength;
	TArray<FGrid> pathArray = gridManager->FindPath(Unit->GetGrid(), Grid, pathLength);

	//


}
