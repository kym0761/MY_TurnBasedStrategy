// Fill out your copyright notice in the Description page of Project Settings.


#include "PathFindingSystem.h"
#include "PathObject.h"

UPathFindingSystem::UPathFindingSystem()
{
	X_Length = 10;
	Y_Length = 10;
}

void UPathFindingSystem::SetPathFindingSystem(int _X_Length, int _Y_Length, TFunctionRef<UPathObject* (UPathFindingSystem*, FGrid)> CreateObjectFunction)
{
	X_Length = _X_Length;
	Y_Length = _Y_Length;


	for (int x = 0; x < X_Length; x++)
	{
		for (int y = 0; y < Y_Length; y++)
		{
			FGrid grid = FGrid(x, y);
			//CreateObjectFunction을 구현해야함. 람다로 구현.
			//GridManager 참고.

			UPathObject* pathObj = CreateObjectFunction(this, grid);
			PathObjectMap.Add(grid,pathObj);
		}
	}
}

TMap<FGrid, UPathObject*> UPathFindingSystem::GetPathObjectMap() const
{
	return PathObjectMap;
}

UPathObject* UPathFindingSystem::GetValidPathObject(const FGrid& Grid) const
{
	if (PathObjectMap.Contains(Grid))
	{
		if (IsValid(PathObjectMap[Grid]))
		{
			return PathObjectMap[Grid];
		}
	}

	return nullptr;
}