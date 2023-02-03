// Fill out your copyright notice in the Description page of Project Settings.


#include "GridSystem.h"
#include "GridObject.h"
/*
* �𸮾� C++ Template�� cpp�� ������ ������ �ʴ´�
* ���⿡ ������ ������ ������ �ִ� ���̴� ����.
* ������ ��� �ȿ��� �ذ��ؾ���.
*/
UGridSystem::UGridSystem()
{
	X_Length = 10;
	Y_Length = 10;
	CellSize = 100.0f;
}
void UGridSystem::SetGridSystem(int _X_Length, int _Y_Length, float _CellSize, TFunctionRef<UGridObject* (UGridSystem*, FGrid)> CreateObjectFunction)
{
	X_Length = _X_Length;
	Y_Length = _Y_Length;
	CellSize = _CellSize;

	for (int x = 0; x < X_Length; x++)
	{
		for (int y = 0; y < Y_Length; y++)
		{
			FGrid grid = FGrid(x, y);
			//CreateObjectFunction�� �����ؾ���. ���ٷ� ����.
			//GridManager ����.

			UGridObject* gridobj = CreateObjectFunction(this, grid);
			GridObjectArray.Add(gridobj);
		}
	}
}

TArray<UGridObject*> UGridSystem::GetGridObjectArray() const
{
	return GridObjectArray;
}

FGrid UGridSystem::WorldToGrid(const FVector& WorldPosition) const
{
	// ?? Is Really Needed??

	FGrid grid;
	grid.X = FMath::RoundToInt(WorldPosition.X / CellSize);
	grid.Y = FMath::RoundToInt(WorldPosition.Y / CellSize);

	return grid;
}

FVector UGridSystem::GridToWorld(const FGrid& Grid) const
{
	// ?? Is Really Needed??

	FVector worldPosition;
	worldPosition.X = Grid.X * CellSize;
	worldPosition.Y = Grid.Y * CellSize;

	return worldPosition;
}

UGridObject* UGridSystem::GetValidGridObject(const FGrid& Grid) const
{
	int32 x = Grid.X;
	int32 y = Grid.Y;

	int32 index = X_Length * x + y;

	if (GridObjectArray.IsValidIndex(index))
	{
		UGridObject* gridObj = GridObjectArray[index];

		if (IsValid(gridObj) && gridObj->GetGrid() == Grid)
		{
			return gridObj;
		}
	}

	return nullptr;
}