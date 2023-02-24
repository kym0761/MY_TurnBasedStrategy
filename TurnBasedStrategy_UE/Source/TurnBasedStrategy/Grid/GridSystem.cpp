// Fill out your copyright notice in the Description page of Project Settings.


#include "GridSystem.h"
#include "GridObject.h"

/*
* 언리얼 C++ Template는 cpp에 구현이 생기지 않는다
* 여기에 구현이 있으면 문제가 있는 것이니 참고.
* 구현은 헤더 안에서 해결해야함.
*/

UGridSystem::UGridSystem()
{
	X_Length = 10;
	Y_Length = 10;
}
void UGridSystem::SetGridSystem(int _X_Length, int _Y_Length, TFunctionRef<UGridObject* (UGridSystem*, FGrid)> CreateObjectFunction)
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

			UGridObject* gridobj = CreateObjectFunction(this, grid);
			GridObjectArray.Add(gridobj);
		}
	}
}

TArray<UGridObject*> UGridSystem::GetGridObjectArray() const
{
	return GridObjectArray;
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