// Fill out your copyright notice in the Description page of Project Settings.


#include "GridSystem.h"
#include "GridObject.h"
#include "GridVisual.h"
#include "Algo/Reverse.h"

// Sets default values
AGridSystem::AGridSystem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bRunConstructionScriptOnDrag = false;
}

// Called when the game starts or when spawned
void AGridSystem::BeginPlay()
{
	Super::BeginPlay();
	
	CreateGridVisual();

	////temp. hide all after 5 sec
	//FTimerHandle timer;
	//GetWorldTimerManager().SetTimer(timer, this, &AGridSystem::HideAllGridVisual, 5.0f, false, 5.0f);

}

void AGridSystem::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	UE_LOG(LogTemp, Warning, TEXT("OnConstruction"));


	bool isGood = true;
	if (GridObjectArray.Num() == (X_Length * Y_Length))
	{
		for (auto gridobject : GridObjectArray)
		{
			if (IsValid(gridobject))
			{
				continue;
			}
			else
			{
				isGood = false;
				break;
			}
		}
	}
	else
	{
		isGood = false;
	}
	
	if(isGood == false)
	{
		GridObjectArray.Empty();

		for (int32 x = 0; x < X_Length; x++)
		{
			for (int32 y = 0; y < Y_Length; y++)
			{
				FGrid grid;
				grid.X = x;
				grid.Y = y;

				//TODO : Height?

				UGridObject* gridObject = NewObject<UGridObject>();
				gridObject->SetGrid(grid);
				GridObjectArray.Add(gridObject);
			}
		}
	}

}

// Called every frame
void AGridSystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool AGridSystem::IsValidGrid(FGrid Grid)
{
	return Grid.X >= 0 && Grid.Y >= 0 && Grid.X < X_Length && Grid.Y < Y_Length;
}

int32 AGridSystem::GetX_Length()
{
	return X_Length;
}

int32 AGridSystem::GetY_Length()
{
	return Y_Length;
}

FGrid AGridSystem::WorldToGrid(FVector WorldPosition)
{
	FGrid grid;
	grid.X = FMath::RoundToInt(WorldPosition.X / CellSize);
	grid.Y = FMath::RoundToInt(WorldPosition.Y / CellSize);

	return grid;
}

FVector AGridSystem::GridToWorld(FGrid Grid)
{
	FVector worldPosition;
	worldPosition.X = Grid.X * CellSize;
	worldPosition.Y = Grid.Y * CellSize;

	return worldPosition;
}

UGridObject* AGridSystem::GetValidGridObject(FGrid Grid)
{
	int32 x = Grid.X;
	int32 y = Grid.Y;

	int32 index = X_Length * x + y;

	if (GridObjectArray.IsValidIndex(index))
	{
		UGridObject* gridObject = GridObjectArray[index];

		if (IsValid(gridObject) && gridObject->GetGrid() == Grid)
		{
			return gridObject;
		}
	}

	return nullptr;
}

AGridVisual* AGridSystem::GetValidGridVisual(FGrid Grid)
{
	int32 x = Grid.X;
	int32 y = Grid.Y;

	int32 index = X_Length * x + y;

	if (GridVisualArray.IsValidIndex(index))
	{
		AGridVisual* gridVisual = GridVisualArray[index];

		if (IsValid(gridVisual))
		{
			return gridVisual;
		}
	}

	return nullptr;
}

void AGridSystem::CreateGridVisual()
{
	if (!IsValid(GridVisualClass))
	{
		UE_LOG(LogTemp, Warning, TEXT("GridVisualClass is not Set."));
		return;
	}


	for (int32 x = 0; x < X_Length; x++)
	{
		for (int32 y = 0; y < Y_Length; y++)
		{
			int32 index = X_Length * x + y;

			if (GridObjectArray.IsValidIndex(index))
			{
				UGridObject* gridObj = GridObjectArray[index];

				FGrid grid = gridObj->GetGrid();

				FVector gridWorldPosition = GridToWorld(grid);

				UE_LOG(LogTemp, Warning, TEXT("Grid position : %s"), *gridWorldPosition.ToString());

				FActorSpawnParameters param;
				param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				AGridVisual* visual = GetWorld()->SpawnActor<AGridVisual>(GridVisualClass, gridWorldPosition, FRotator::ZeroRotator, param);
				GridVisualArray.Add(visual);

			}
		}
	}

}

void AGridSystem::ShowAllGridVisual()
{
	for (auto gridVisual : GridVisualArray)
	{
		gridVisual->Show();
	}
}

void AGridSystem::HideAllGridVisual()
{
	for (auto gridVisual : GridVisualArray)
	{
		gridVisual->Hide();
	}
}

void AGridSystem::ShowGridRange(FGrid Grid, int32 Range, EGridVisualType GridVisualType)
{
	TArray<FGrid> gridList;

	for (int32 x = -Range; x <= Range; x++)
	{
		for (int32 y = -Range; y <= Range; y++)
		{
			FGrid temp = Grid + FGrid(x,y);

			if (!IsValidGrid(temp))
			{
				continue;
			}

			int32 distance = FMath::Abs(x) + FMath::Abs(y);
			if (distance > Range)
			{
				continue;
			}

			gridList.Add(temp);
		}
	}

	ShowFromGridList(gridList, GridVisualType);
}

void AGridSystem::ShowFromGridList(TArray<FGrid> GridList, EGridVisualType GridVisualType)
{
	for (FGrid grid : GridList)
	{
		AGridVisual* gridVisual = GetValidGridVisual(grid);
		if (IsValid(gridVisual))
		{
			gridVisual->Show();
		}
	}
}

TArray<FGrid> AGridSystem::FindPath(FGrid Start, FGrid End, int32& PathLength)
{

	TArray<UPathNode*> openList;
	TArray<UPathNode*> closeList;

	UPathNode* startNode = NewObject<UPathNode>();
	startNode->SetGrid(Start);

	openList.Add(startNode);

	UPathNode* endNode = NewObject<UPathNode>();
	endNode->SetGrid(End);

	for (int x = 0; x < X_Length; x++)
	{
		for (int y = 0; y < Y_Length; y++)
		{
			UPathNode* pathNode = NewObject<UPathNode>();
			startNode->SetGrid(Start);

			pathNode->SetGCost(TNumericLimits<int>::Max());
			pathNode->SetHCost(0);
			pathNode->CalculateFCost();
		}
	}

	startNode->SetGCost(0);
	startNode->SetHCost(CalculateGridDistance(Start, End));
	startNode->CalculateFCost();

	while (openList.Num()>0)
	{
		UPathNode* currentNode = GetLowestFCostNode(openList);

		if (currentNode == endNode)
		{
			PathLength = endNode->GetFCost();
			return CalculatePath(endNode);
		}

		openList.Remove(currentNode);
		closeList.Add(currentNode);

		for (UPathNode* nearNode : GetNearNodeList(currentNode))
		{
			if (closeList.Contains(nearNode))
			{
				continue;
			}

			if (nearNode->GetIsWalkable() == false)
			{
				closeList.Add(nearNode);
				continue;
			}

			int tempGCost = currentNode->GetGCost() + CalculateGridDistance(currentNode->GetGrid(), nearNode->GetGrid());

			if (tempGCost < nearNode->GetGCost())
			{
				nearNode->SetParentNode(currentNode);
				nearNode->SetGCost(tempGCost);
				nearNode->SetHCost(CalculateGridDistance(nearNode->GetGrid(), End));
				nearNode->CalculateFCost();

				if (!openList.Contains(nearNode))
				{
					openList.Add(nearNode);
				}

			}


		}



	}



	PathLength = 0;
	return TArray<FGrid>();
}

int32 AGridSystem::CalculateGridDistance(FGrid a, FGrid b)
{
	FGrid grid = a - b;

	return FMath::Abs(grid.X) + FMath::Abs(grid.Y);
}

UPathNode* AGridSystem::GetLowestFCostNode(TArray<UPathNode*> PathNodeList)
{
	if (PathNodeArray.Num() == 0)
	{
		return nullptr;
	}

	UPathNode* pathNode = PathNodeList[0];
	for (auto node : PathNodeArray)
	{
		if (node->GetFCost() < pathNode->GetFCost())
		{
			pathNode = node;
		}
	}

	return pathNode;
}

TArray<FGrid> AGridSystem::CalculatePath(UPathNode* EndNode)
{
	TArray<FGrid> gridArray;

	UPathNode* current = EndNode;
	while (IsValid(current))
	{
		gridArray.Add(current->GetGrid());
		current = current->GetParentNode();
	}

	Algo::Reverse(gridArray);
	
	return gridArray;
}

TArray<UPathNode*> AGridSystem::GetNearNodeList(UPathNode* CurrentNode)
{
	TArray<UPathNode*> nearNodeList;

	FGrid grid = CurrentNode->GetGrid();

	FGrid upGrid = grid + FGrid(1, 0);
	FGrid downGrid = grid + FGrid(-1, 0);
	FGrid rightGrid = grid + FGrid(0, 1);
	FGrid leftGrid = grid + FGrid(0, -1);

	TArray<FGrid> nearGrid;

	nearGrid.Add(upGrid);
	nearGrid.Add(downGrid);
	nearGrid.Add(rightGrid);
	nearGrid.Add(leftGrid);

	for (auto near : nearGrid)
	{
		if (IsValidGrid(near))
		{
			UPathNode* nearNode = NewObject<UPathNode>();
			nearNode->SetGrid(near);

			nearNodeList.Add(nearNode);
		}
	}

	return nearNodeList;
}
