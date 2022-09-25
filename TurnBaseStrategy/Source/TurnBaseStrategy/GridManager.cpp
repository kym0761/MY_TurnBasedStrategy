// Fill out your copyright notice in the Description page of Project Settings.


#include "GridManager.h"
#include "GridObject.h"
#include "PathNode.h"
#include "InstancedGridVisualComponent.h"

#include "Kismet/GameplayStatics.h"
#include "PathFindingSystem.h"
#include "GridSystem.h"

// Sets default values
AGridManager::AGridManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bRunConstructionScriptOnDrag = false;

	X_Length = 10;
	Y_Length = 10;
	CellSize = 100.0f;


		GridVisual_OK = CreateDefaultSubobject<UInstancedGridVisualComponent>(TEXT("GridVisual_OK"));


		GridVisual_NO = CreateDefaultSubobject<UInstancedGridVisualComponent>(TEXT("GridVisual_NO"));

}

// Called when the game starts or when spawned
void AGridManager::BeginPlay()
{
	Super::BeginPlay();

	CreateGridSystem();

	//CreateGridVisual();
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
	GridSystem = NewObject<UGridSystem>();

	GridSystem->SetGridSystem(X_Length, Y_Length, CellSize, [](UGridSystem* gs, FGrid grid) {
		UGridObject* gridObj = NewObject<UGridObject>();
		gridObj->SetGrid(grid);
		gridObj->SetGridSystem(gs);
		return gridObj;
		});

	PathFindingGridSystem = NewObject<UPathFindingSystem>();

	PathFindingGridSystem->SetPathFindingSystem(X_Length, Y_Length, CellSize,
		[](UPathFindingSystem* pfs, FGrid grid)
		{
			UPathNode* pathNode = NewObject<UPathNode>();
			pathNode->SetGrid(grid);
			return pathNode;
		}
	);
}

//void AGridManager::CreateGridVisual()
//{
//	if (!IsValid(GridVisualClass))
//	{
//		UE_LOG(LogTemp, Warning, TEXT("GridVisualClass is not Set."));
//		return;
//	}
//
//	if (!GridSystem.IsValid())
//	{
//		UE_LOG(LogTemp, Warning, TEXT("GridSystem is not Set."));
//		return;
//	}
//
//	TArray<UGridObject*> gridObjectArray = GridSystem->GetObjectArray();
//
//	int32 num = gridObjectArray.Num();
//	UE_LOG(LogTemp, Warning, TEXT("Num : %d"), num);
//
//	for (int32 x = 0; x < X_Length; x++)
//	{
//		for (int32 y = 0; y < Y_Length; y++)
//		{
//			int32 index = Y_Length * x + y;
//
//			if (gridObjectArray.IsValidIndex(index))
//			{
//				UGridObject* gridObj = gridObjectArray[index];
//
//				FGrid grid = gridObj->GetGrid();
//
//				UE_LOG(LogTemp, Warning, TEXT("Grid : %s"), *grid.ToString());
//
//
//				FVector gridWorldPosition = GridSystem->GridToWorld(grid);
//
//				UE_LOG(LogTemp, Warning, TEXT("Grid position : %s"), *gridWorldPosition.ToString());
//
//				FActorSpawnParameters param;
//				param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
//				AGridVisual* visual = GetWorld()->SpawnActor<AGridVisual>(GridVisualClass, gridWorldPosition, FRotator::ZeroRotator, param);
//				GridVisualArray.Add(visual);
//
//			}
//		}
//	}
//}

//void AGridManager::ShowAllGridVisual()
//{
//	
//
//	//GridVisual_OK->RemoveGridVisuals();
//	//GridVisual_NO->RemoveGridVisuals();
//	//for (AGridVisual* gridVisual : GridVisualArray)
//	//{
//	//	gridVisual->Show();
//	//}
//}

void AGridManager::HideAllGridVisual()
{
	GridVisual_OK->RemoveGridVisuals();
	GridVisual_NO->RemoveGridVisuals();

	//for (AGridVisual* gridVisual : GridVisualArray)
	//{
	//	gridVisual->Hide();
	//}
}

FGrid AGridManager::WorldToGrid(FVector WorldPosition)
{

	return GridSystem->WorldToGrid(WorldPosition);
}

FVector AGridManager::GridToWorld(FGrid Grid)
{

	return GridSystem->GridToWorld(Grid);

}

UGridObject* AGridManager::GetValidGridObject(FGrid Grid)
{

	UGridObject* gridObj = GridSystem->GetValidGridObject(Grid);

	if (gridObj->GetGrid() == Grid)
	{
		return gridObj;
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

	ShowFromGridArray(gridList, GridVisualType);
}

void AGridManager::ShowFromGridArray(TArray<FGrid> GridArray, EGridVisualType GridVisualType)
{
	GridVisual_OK->DrawGridVisualswithGridArray(GridArray);


	//for (FGrid grid : GridArray)
	//{
	//	AGridVisual* gridVisual = GetValidGridVisual(grid);
	//	if (IsValid(gridVisual))
	//	{
	//		gridVisual->Show();
	//	}
	//}
}

TArray<FGrid> AGridManager::FindPath(FGrid Start, FGrid End, int32& PathLength)
{
	TArray<UPathNode*> openList;
	TArray<UPathNode*> closeList;

	UPathNode* startNode = PathFindingGridSystem->GetValidPathNode(Start);

	if (!IsValid(startNode))
	{
		UE_LOG(LogTemp, Warning, TEXT("StartNode Not Valid"));
		PathLength = -1;
		return TArray<FGrid>();
	}

	openList.Add(startNode);

	UPathNode* endNode = PathFindingGridSystem->GetValidPathNode(End);
	if (!IsValid(endNode))
	{
		UE_LOG(LogTemp, Warning, TEXT("EndNode Not Valid"));
		PathLength = -1;
		return TArray<FGrid>();
	}

	InitAllPathFindingNodes();

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

		TArray<UPathNode*> nearNodeArray = GetNearNodeArray(currentNode);
		for (UPathNode* nearNode : nearNodeArray)
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

			auto gridObj = GridSystem->GetValidGridObject(nearNode->GetGrid());
			if (IsValid(gridObj) && gridObj->HasAnyUnit())
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

	PathLength = -1;
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

TArray<UPathNode*> AGridManager::GetNearNodeArray(UPathNode* CurrentNode)
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
			UPathNode* nearNode = PathFindingGridSystem->GetValidPathNode(near);

			if (!IsValid(nearNode))
			{
				continue;
			}

			nearNodeList.Add(nearNode);
		}
	}

	return nearNodeList;
}

void AGridManager::AddUnitAtGrid(FGrid GridValue, AUnitCharacter* Unit)
{

	auto gridObject = GridSystem->GetValidGridObject(GridValue);
	gridObject->AddUnit(Unit);

}

TArray<AUnitCharacter*> AGridManager::GetUnitArrayAtGrid(FGrid GridValue)
{

	UGridObject* gridObject = GridSystem->GetValidGridObject(GridValue);
	if (!IsValid(gridObject))
	{
		return TArray<AUnitCharacter*>();
	}


	return gridObject->GetUnitArray();

}

bool AGridManager::HasAnyUnitOnGrid(FGrid GridValue)
{

	auto gridObj = GridSystem->GetValidGridObject(GridValue);
	if (IsValid(gridObj))
	{
		return gridObj->HasAnyUnit();
	}
	
	return false;
}

bool AGridManager::HasPath(FGrid Start, FGrid End)
{
	int32 pathLength = 0;
	FindPath(Start, End, pathLength);

	return pathLength != -1;
}

bool AGridManager::IsWalkableGrid(FGrid GridValue)
{
	UPathNode* pathNode = PathFindingGridSystem->GetValidPathNode(GridValue);
	if (IsValid(pathNode))
	{
		return pathNode->GetIsWalkable();
	}

	return false;
}

int32 AGridManager::GetPathLength(FGrid Start, FGrid End)
{
	int32 pathLength = 0;
	FindPath(Start,End, pathLength);
	return pathLength;
}

void AGridManager::InitAllPathFindingNodes()
{
	//PathFindingGridSystem의 Grid 값을 전부 초기화.
	for (int x = 0; x < X_Length; x++)
	{
		for (int y = 0; y < Y_Length; y++)
		{
			FGrid grid = FGrid(x, y);
			UPathNode* pathNode = PathFindingGridSystem->GetValidPathNode(grid);

			if (!IsValid(pathNode))
			{
				UE_LOG(LogTemp, Warning, TEXT("PathNode is Not Valid"));
			}

			pathNode->SetGCost(TNumericLimits<int32>::Max());
			pathNode->SetHCost(0);
			pathNode->CalculateFCost();
			pathNode->SetParentNode(nullptr);
		}
	}
}

AGridManager* AGridManager::GetGridManager()
{
	if (!IsValid(GEngine) || GEngine->GameViewport.IsNull())
	{
		return nullptr;
	}

	UWorld* world = GEngine->GameViewport->GetWorld();
	AGridManager* gridManager = Cast<AGridManager>(UGameplayStatics::GetActorOfClass(world, AGridManager::StaticClass()));

	return gridManager;
}



