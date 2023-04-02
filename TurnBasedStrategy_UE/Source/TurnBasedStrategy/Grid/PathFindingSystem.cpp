// Fill out your copyright notice in the Description page of Project Settings.


#include "PathFindingSystem.h"
#include "PathNode.h"

UPathFindingSystem::UPathFindingSystem()
{
	X_Length = 10;
	Y_Length = 10;
}

void UPathFindingSystem::SetPathFindingSystem(int _X_Length, int _Y_Length, TFunctionRef<UPathNode* (UPathFindingSystem*, FGrid)> CreateObjectFunction)
{
	X_Length = _X_Length;
	Y_Length = _Y_Length;


	for (int x = 0; x < X_Length; x++)
	{
		for (int y = 0; y < Y_Length; y++)
		{
			FGrid grid = FGrid(x, y);
			//CreateObjectFunction�� �����ؾ���. ���ٷ� ����.
			//GridManager ����.

			UPathNode* pathNode = CreateObjectFunction(this, grid);
			//PathNodeArray.Add(pathNode);
			PathNodeMap.Add(grid,pathNode);
		}
	}
}

TMap<FGrid, UPathNode*> UPathFindingSystem::GetPathNodeMap() const
{
	return PathNodeMap;
}

UPathNode* UPathFindingSystem::GetValidPathNode(const FGrid& Grid) const
{
	if (PathNodeMap.Contains(Grid))
	{
		if (IsValid(PathNodeMap[Grid]))
		{
			return PathNodeMap[Grid];
		}
	}

	return nullptr;
}