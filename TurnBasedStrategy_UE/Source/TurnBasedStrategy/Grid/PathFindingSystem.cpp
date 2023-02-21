// Fill out your copyright notice in the Description page of Project Settings.


#include "PathFindingSystem.h"
#include "PathNode.h"

UPathFindingSystem::UPathFindingSystem()
{
	X_Length = 10;
	Y_Length = 10;
	//CellSize = 100.0f;
}

void UPathFindingSystem::SetPathFindingSystem(int _X_Length, int _Y_Length, TFunctionRef<UPathNode* (UPathFindingSystem*, FGrid)> CreateObjectFunction)
{
	X_Length = _X_Length;
	Y_Length = _Y_Length;
	//CellSize = _CellSize;

	for (int x = 0; x < X_Length; x++)
	{
		for (int y = 0; y < Y_Length; y++)
		{
			FGrid grid = FGrid(x, y);
			//CreateObjectFunction을 구현해야함. 람다로 구현.
			//GridManager 참고.

			UPathNode* pathNode = CreateObjectFunction(this, grid);
			PathNodeArray.Add(pathNode);
		}
	}
}

TArray<UPathNode*> UPathFindingSystem::GetPathNodeArray() const
{
	return PathNodeArray;
}

//FGrid UPathFindingSystem::WorldToGrid(const FVector& WorldPosition) const
//{
//	// ?? Is Really Needed??
//	// GridManager 안에 이미 존재함.
//
//	FGrid grid;
//	grid.X = FMath::RoundToInt(WorldPosition.X / CellSize);
//	grid.Y = FMath::RoundToInt(WorldPosition.Y / CellSize);
//
//	return grid;
//}
//
//FVector UPathFindingSystem::GridToWorld(const FGrid& Grid) const
//{
//	// ?? Is Really Needed??
//	// GridManager 안에 이미 존재함.
//
//	FVector worldPosition;
//	worldPosition.X = Grid.X * CellSize;
//	worldPosition.Y = Grid.Y * CellSize;
//
//	return worldPosition;
//}

UPathNode* UPathFindingSystem::GetValidPathNode(const FGrid& Grid) const
{
	int32 x = Grid.X;
	int32 y = Grid.Y;

	int32 index = X_Length * x + y;

	if (PathNodeArray.IsValidIndex(index))
	{
		UPathNode* pathNode = PathNodeArray[index];

		if (IsValid(pathNode) && pathNode->GetGrid() == Grid)
		{
			return pathNode;
		}
	}

	return nullptr;
}
