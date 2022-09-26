// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitMoveActionComponent.h"
#include "UnitCharacter.h"

#include "GridManager.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"

UUnitMoveActionComponent::UUnitMoveActionComponent()
{
	MaxActionRange = 5;
}

void UUnitMoveActionComponent::BeginPlay()
{
	Super::BeginPlay();

}

FString UUnitMoveActionComponent::GetActionName() const
{
	return FString("Move");
}

TArray<FGrid> UUnitMoveActionComponent::GetValidActionGridArray() const
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

			FGrid testGrid = unitGrid + offsetGrid;


			//존재하지 않는 Grid
			if (!gridManager->IsValidGrid(testGrid))
			{
				continue;
			}
			
			////지금 현재 Unit의 위치
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


	return validArray;
}

TArray<FGridVisualData> UUnitMoveActionComponent::GetValidActionGridVisualDataArray() const
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

			FGridVisualData testData;
			testData.Grid = resultGrid;
			testData.GridVisualType = EGridVisualType::Move;

			////지금 현재 Unit의 위치
			if (resultGrid == unitGrid)
			{
				testData.GridVisualType = EGridVisualType::Warning;
			}

			//누군가 점유중이면 Skip
			if (gridManager->HasAnyUnitOnGrid(resultGrid))
			{
				testData.GridVisualType = EGridVisualType::NO;
			}

			//걸을 수 있는 위치?
			if (!gridManager->IsWalkableGrid(resultGrid))
			{
				testData.GridVisualType = EGridVisualType::NO;
			}

			//도착 가능한 위치?
			if (!gridManager->HasPath(unitGrid, resultGrid))
			{
				testData.GridVisualType = EGridVisualType::NO;

			}

			//의도와 달리 먼 거리?
			if (gridManager->GetPathLength(unitGrid, resultGrid) > MaxActionRange)
			{
				testData.GridVisualType = EGridVisualType::NO;
			}

			//통과하면 문제없으니 validArray에 추가

			validVisualDataArray.Add(testData);
		}
	}


	return validVisualDataArray;
}

void UUnitMoveActionComponent::TakeAction(FGrid Grid)
{
	AGridManager* gridManager = AGridManager::GetGridManager();
	if (!IsValid(gridManager))
	{
		return;
	}

	int32 pathLength;
	TArray<FGrid> pathArray = gridManager->FindPath(Unit->GetGrid(), Grid, pathLength);

	if (pathLength > MaxActionRange)
	{
		UE_LOG(LogTemp, Warning, TEXT("Not Valid MovePosition"));
		return;
	}

	for (int i = 0; i < pathArray.Num(); i++)
	{
		DrawDebugSphere(GetWorld(), gridManager->GridToWorld(pathArray[i]), 10, 12, FColor::Blue, false, 1.5f, 0, 2.0f);
	}

	Cast<AAIController>(Cast<ACharacter>(GetOwner())->GetController())->MoveToLocation(gridManager->GridToWorld(pathArray.Last()));
	gridManager->MoveUnitGrid(Unit, Unit->GetGrid(), pathArray.Last());
	gridManager->RemoveAllGridVisual();
	if (OnStartMoving.IsBound())
	{
		OnStartMoving.Broadcast();
	}

}
