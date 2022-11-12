// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitMoveActionComponent.h"
#include "UnitCharacter.h"

#include "GridManager.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"

UUnitMoveActionComponent::UUnitMoveActionComponent()
{
	MaxActionRange = 5;

	ActionName = FString("Move");
}

void UUnitMoveActionComponent::BeginPlay()
{
	Super::BeginPlay();

}

TArray<FGrid> UUnitMoveActionComponent::GetValidActionGridArray() const
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

			//누군가 점유중이면 Skip
			if (gridManager->HasAnyUnitOnGrid(resultGrid))
			{
				continue;
			}

			//걸을 수 있는 위치?
			if (!gridManager->IsWalkableGrid(resultGrid))
			{
				continue;
			}

			bool bisFriend = false;
			AUnitCharacter* targetUnit = gridManager->GetUnitAtGrid(resultGrid);
			if (IsValid(targetUnit) && GetOwner()->Tags.Num() > 0)
			{
				bisFriend = targetUnit->ActorHasTag(GetOwner()->Tags[0]);
			}

			//도착 가능한 위치?
			if (bisFriend && !gridManager->HasPath(unitGrid, resultGrid, true) || !gridManager->HasPath(unitGrid, resultGrid))
			{
				continue;
			}

			//의도와 달리 먼 거리?
			if (gridManager->GetPathLength(unitGrid, resultGrid) > MaxActionRange)
			{
				continue;
			}

			//통과하면 문제없으니 validArray에 추가

			validArray.Add(resultGrid);
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
			
			//존재하지 않는 Grid
			if (!gridManager->IsValidGrid(resultGrid))
			{
				continue;
			}

			FGridVisualData testData;
			testData.Grid = resultGrid;
			testData.GridVisualType = EGridVisualType::Move;

			//지금 현재 Unit의 위치
			if (resultGrid == unitGrid)
			{
				testData.GridVisualType = EGridVisualType::Warning;
			}

			//누군가 점유중
			if (gridManager->HasAnyUnitOnGrid(resultGrid))
			{
				testData.GridVisualType = EGridVisualType::NO;
			}

			//걸을 수 있는 위치?
			if (!gridManager->IsWalkableGrid(resultGrid))
			{
				testData.GridVisualType = EGridVisualType::NO;
			}

			bool bisFriend = false;
			auto targetUnit = gridManager->GetUnitAtGrid(resultGrid);
			if (IsValid(targetUnit) && GetOwner()->Tags.Num()>0)
			{
				bisFriend = targetUnit->ActorHasTag(GetOwner()->Tags[0]);
			}

			//도착 가능한 위치?
			if (bisFriend &&!gridManager->HasPath(unitGrid, resultGrid,true) || !gridManager->HasPath(unitGrid, resultGrid))
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
		UE_LOG(LogTemp, Warning, TEXT("it's over MaxRange to Move here."));
		return;
	}

	if (pathLength == -1)
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't reach to the Target Grid. there is no Path"));
		return;
	}

	if (!pathArray.Contains(Grid))
	{
		UE_LOG(LogTemp, Warning, TEXT("You Can't Move to Invalid Grid"));
		return;
	}


	//~~ Move Debug.

	for (int i = 0; i < pathArray.Num(); i++)
	{
		DrawDebugSphere(GetWorld(), gridManager->GridToWorld(pathArray[i]), 10, 12, FColor::Blue, false, 1.5f, 0, 2.0f);
	}

	//a simple temp Move Function.
	auto aiController = Cast<AAIController>(Unit->GetController());
	if (!IsValid(aiController))
	{
		return;
	}
	aiController->MoveToLocation(gridManager->GridToWorld(pathArray.Last()));
	


	//Update Grid Data
	gridManager->MoveUnitGrid(Unit, Unit->GetGrid(), pathArray.Last());
	gridManager->RemoveAllGridVisual();
	if (OnStartMoving.IsBound())
	{
		OnStartMoving.Broadcast();
	}

}
