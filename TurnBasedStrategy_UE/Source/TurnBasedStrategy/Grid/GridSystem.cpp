// Fill out your copyright notice in the Description page of Project Settings.


#include "GridSystem.h"
#include "GridObject.h"
#include "UnitCore/UnitCharacter.h"
/*
* �𸮾� C++ Template�� cpp�� ������ ������ �ʴ´�
* ���⿡ ������ ������ ������ �ִ� ���̴� ����.
* ������ ��� �ȿ��� �ذ��ؾ���.
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
			//CreateObjectFunction�� �����ؾ���. ���ٷ� ����.
			//GridManager ����.

			UGridObject* gridobj = CreateObjectFunction(this, grid);
			//GridObjectMap.Add(gridobj);
			GridObjectMap.Add(grid, gridobj);
		}
	}
}

TMap<FGrid, UGridObject*> UGridSystem::GetGridObjectMap() const
{
	return GridObjectMap;
}

UGridObject* UGridSystem::GetValidGridObject(const FGrid& Grid) const
{
	if (GridObjectMap.Contains(Grid))
	{
		if (IsValid(GridObjectMap[Grid]))
		{
			return GridObjectMap[Grid];
		}
	}

	return nullptr;
}


TMap<FGrid, UGridObject*> UGridSystem::GetAllGridObjectsThatHasUnit() const
{
	TMap<FGrid, UGridObject*> resultMap;
	
	for (TPair<FGrid,UGridObject*> gridPair : GridObjectMap)
	{
		AUnitCharacter* unit = gridPair.Value->GetUnit();
		if (IsValid(unit))
		{
			resultMap.Add(gridPair);
		}
	}

	return resultMap;
}