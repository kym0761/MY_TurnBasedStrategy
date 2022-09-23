// Fill out your copyright notice in the Description page of Project Settings.


#include "GridManager.h"
#include "GridObject.h"
#include "PathNode.h"




// Sets default values
AGridManager::AGridManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bRunConstructionScriptOnDrag = false;

	X_Length = 10;
	Y_Length = 10;
	CellSize = 100.0f;
}

// Called when the game starts or when spawned
void AGridManager::BeginPlay()
{
	Super::BeginPlay();

	CreateGridSystem();

	CreateGridVisual();
}

void AGridManager::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);


	//auto gs = new FGridSystem<UGridObject>();

	//GridSystem = MakeShareable(gs);


	//bool isGood = true;
	//if (GridObjectArray.Num() == (X_Length * Y_Length))
	//{
	//	for (auto gridobject : GridObjectArray)
	//	{
	//		if (IsValid(gridobject))
	//		{
	//			continue;
	//		}
	//		else
	//		{
	//			isGood = false;
	//			break;
	//		}
	//	}
	//}
	//else
	//{
	//	isGood = false;
	//}

	//if (isGood == false)
	//{
	//	GridObjectArray.Empty();

	//	for (int32 x = 0; x < X_Length; x++)
	//	{
	//		for (int32 y = 0; y < Y_Length; y++)
	//		{
	//			FGrid grid;
	//			grid.X = x;
	//			grid.Y = y;

	//			//TODO : Height?

	//			UGridObject* gridObject = NewObject<UGridObject>();
	//			gridObject->SetGrid(grid);
	//			GridObjectArray.Add(gridObject);
	//		}
	//	}
	//}
}

// Called every frame
void AGridManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool AGridManager::IsValidGrid(FGrid Grid)
{
	return Grid.X >= 0 && Grid.Y >= 0 && Grid.X < X_Length&& Grid.Y < Y_Length;
}

void AGridManager::CreateGridSystem()
{
	auto gridSystem = new FGridSystem<UGridObject>(X_Length, Y_Length, CellSize, [](FGridSystem<UGridObject>* gs, FGrid grid) {
		UGridObject* gridObj = NewObject<UGridObject>();
		gridObj->SetGrid(grid);
		gridObj->SetGridSystem(MakeShareable(gs));
		return gridObj;
		});

	if (GridSystem.IsValid())
	{
		GridSystem.Reset();
	}

	GridSystem = MakeShareable(gridSystem);

	if (GridSystem.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("GridSystem is OK"));

		//for (auto i : GridSystem->GridObjectArray)
		//{
		//	UE_LOG(LogTemp, Warning, TEXT("GridObject : %s"), *i->ToString());
		//	UE_LOG(LogTemp, Warning, TEXT("Grid : %s"), *i->GetGrid().ToString());
		//}
	}

	auto pathFindingGridSystem = new FGridSystem<UPathNode>(X_Length, Y_Length, CellSize, [](FGridSystem<UPathNode>* gs, FGrid grid) {
		UPathNode* pathNode = NewObject<UPathNode>();
		pathNode->SetGrid(grid);
		return pathNode;
		});

	if (PathFindingGridSystem.IsValid())
	{
		PathFindingGridSystem.Reset();
		//UE_LOG(LogTemp, Warning, TEXT("PathFindingGridSystem is Reset"));
	}

	PathFindingGridSystem = MakeShareable(pathFindingGridSystem);

	UE_LOG(LogTemp, Warning, TEXT("CreateGridSystem"));
}

void AGridManager::CreateGridVisual()
{
	if (!IsValid(GridVisualClass))
	{
		UE_LOG(LogTemp, Warning, TEXT("GridVisualClass is not Set."));
		return;
	}

	if (!GridSystem.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("GridSystem is not Set."));
		return;
	}

	TArray<UGridObject*> gridObjectArray = GridSystem->GetObjectArray();

	for (int32 x = 0; x < X_Length; x++)
	{
		for (int32 y = 0; y < Y_Length; y++)
		{
			int32 index = Y_Length * x + y;

			if (gridObjectArray.IsValidIndex(index))
			{
				UGridObject* gridObj = gridObjectArray[index];

				FGrid grid = gridObj->GetGrid();

				UE_LOG(LogTemp, Warning, TEXT("Grid : %s"), *grid.ToString());


				FVector gridWorldPosition = GridSystem->GridToWorld(grid);

				UE_LOG(LogTemp, Warning, TEXT("Grid position : %s"), *gridWorldPosition.ToString());

				FActorSpawnParameters param;
				param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				AGridVisual* visual = GetWorld()->SpawnActor<AGridVisual>(GridVisualClass, gridWorldPosition, FRotator::ZeroRotator, param);
				GridVisualArray.Add(visual);

			}
		}
	}
}

void AGridManager::ShowAllGridVisual()
{
	for (AGridVisual* gridVisual : GridVisualArray)
	{
		gridVisual->Show();
	}
}

void AGridManager::HideAllGridVisual()
{
	for (AGridVisual* gridVisual : GridVisualArray)
	{
		gridVisual->Hide();
	}
}

FGrid AGridManager::WorldToGrid(FVector WorldPosition)
{
	if (GridSystem.IsValid())
	{
		return GridSystem->WorldToGrid(WorldPosition);
	}

	return FGrid();
}

FVector AGridManager::GridToWorld(FGrid Grid)
{
	if (GridSystem.IsValid())
	{
		return GridSystem->GridToWorld(Grid);
	}

	return FVector::ZeroVector;
}

UGridObject* AGridManager::GetValidGridObject(FGrid Grid)
{
	if (GridSystem.IsValid())
	{
		UGridObject* gridObj = GridSystem->GetValidGridObject(Grid);

		if (gridObj->GetGrid() == Grid)
		{
			return gridObj;
		}
	}

	return nullptr;
}

AGridVisual* AGridManager::GetValidGridVisual(FGrid Grid)
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

void AGridManager::ShowGridRange(FGrid Grid, int32 Range, EGridVisualType GridVisualType)
{
	TArray<FGrid> gridList;

	for (int32 x = -Range; x <= Range; x++)
	{
		for (int32 y = -Range; y <= Range; y++)
		{
			FGrid temp = Grid + FGrid(x, y);

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

void AGridManager::ShowFromGridList(TArray<FGrid> GridList, EGridVisualType GridVisualType)
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

TArray<FGrid> AGridManager::FindPath(FGrid Start, FGrid End, int32& PathLength)
{
	TArray<UPathNode*> openList;
	TArray<UPathNode*> closeList;

	UPathNode* startNode = PathFindingGridSystem->GetValidGridObject(Start);
	openList.Add(startNode);

	UPathNode* endNode = PathFindingGridSystem->GetValidGridObject(End);

	//PathFindingGridSystem의 Grid 값을 전부 초기화.
	for (int x = 0; x < X_Length; x++)
	{
		for (int y = 0; y < Y_Length; y++)
		{
			FGrid grid = FGrid(x, y);
			UPathNode* pathNode = PathFindingGridSystem->GetValidGridObject(grid);

			pathNode->SetGCost(TNumericLimits<int32>::Max());
			pathNode->SetHCost(0);
			pathNode->CalculateFCost();
			pathNode->SetParentNode(nullptr);
		}
	}

	startNode->SetGCost(0);
	startNode->SetHCost(CalculateGridDistance(Start, End));
	startNode->CalculateFCost();

	while (openList.Num() > 0)
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

int32 AGridManager::CalculateGridDistance(FGrid a, FGrid b)
{
	FGrid grid = a - b;

	return FMath::Abs(grid.X) + FMath::Abs(grid.Y);
}

UPathNode* AGridManager::GetLowestFCostNode(TArray<UPathNode*> PathNodeList)
{
	if (PathNodeList.Num() == 0)
	{
		return nullptr;
	}

	UPathNode* pathNode = PathNodeList[0];
	for (UPathNode* node : PathNodeList)
	{
		if (node->GetFCost() < pathNode->GetFCost())
		{
			pathNode = node;
		}
	}

	return pathNode;
}

TArray<FGrid> AGridManager::CalculatePath(UPathNode* EndNode)
{
	if (!IsValid( EndNode))
	{
		return TArray<FGrid>();
	}

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

TArray<UPathNode*> AGridManager::GetNearNodeList(UPathNode* CurrentNode)
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
			UPathNode* nearNode = PathFindingGridSystem->GetValidGridObject(near);
			//nearNode->SetGrid(near);

			if (!IsValid(nearNode))
			{
				continue;
			}

			nearNodeList.Add(nearNode);
		}
	}

	return nearNodeList;
}



