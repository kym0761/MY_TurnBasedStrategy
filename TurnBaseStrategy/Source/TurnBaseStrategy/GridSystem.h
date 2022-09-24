// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "UObject/NoExportTypes.h"
#include "Grid.h"
#include "GridObject.h"
//#include "GridSystem.generated.h"
/**
 * https://docs.unrealengine.com/5.0/en-US/API/Runtime/Core/GenericPlatform/TFunction/
 * https://stackoverflow.com/questions/1639797/template-issue-causes-linker-error-c
 */

class UGridObject;

template<typename T>
class TURNBASESTRATEGY_API FGridSystem
{

private:

	int32 X_Length;
	int32 Y_Length;
	float CellSize;

public:
	TArray<T*> ObjectArray;

public:

	FGridSystem();
	~FGridSystem();
	FGridSystem(int _X_Length, int _Y_Length, float _CellSize);

	void SetGridSystem(int _X_Length, int _Y_Length, float _CellSize, TSharedPtr<FGridSystem<T>> SharedPtr,
		TFunctionRef<T* (TSharedPtr<FGridSystem<T>>, FGrid)> CreateObjectFunction);

	TArray<T*> GetObjectArray() const;

	FGrid WorldToGrid(FVector WorldPosition) const;
	FVector GridToWorld(FGrid Grid) const;

	T* GetValidGridObject(FGrid Grid) const;
};

template<typename T>
FGridSystem<T>::FGridSystem()
{
	X_Length = 10;
	Y_Length = 10;
	CellSize = 100.0f;
}

template<typename T>
FGridSystem<T>::~FGridSystem()
{
	//UE_LOG(LogTemp, Warning, TEXT("~FGridSystem Called"));
	ObjectArray.Empty();
}

template<typename T>
FGridSystem<T>::FGridSystem(int _X_Length, int _Y_Length, float _CellSize)
{
	X_Length = _X_Length;
	Y_Length = _Y_Length;
	CellSize = _CellSize;
}

template<typename T>
void FGridSystem<T>::SetGridSystem(int _X_Length, int _Y_Length, float _CellSize, TSharedPtr<FGridSystem<T>> SharedPtr,
	TFunctionRef<T* (TSharedPtr<FGridSystem<T>>, FGrid)> CreateObjectFunction)
{
	X_Length = _X_Length;
	Y_Length = _Y_Length;
	CellSize = _CellSize;

	for (int x = 0; x < X_Length; x++)
	{
		for (int y = 0; y < Y_Length; y++)
		{
			FGrid grid = FGrid(x, y);
			//CreateObjectFunction을 구현해야함. 람다로 구현.
			//GridManager 참고.

			T* gridobj = CreateObjectFunction(SharedPtr, grid);
			ObjectArray.Add(gridobj);
		}
	}
}

template<typename T>
inline TArray<T*> FGridSystem<T>::GetObjectArray() const
{
	return ObjectArray;
}

template<typename T>
inline FGrid FGridSystem<T>::WorldToGrid(FVector WorldPosition) const
{
	FGrid grid;
	grid.X = FMath::RoundToInt(WorldPosition.X / CellSize);
	grid.Y = FMath::RoundToInt(WorldPosition.Y / CellSize);

	return grid;
}

template<typename T>
inline FVector FGridSystem<T>::GridToWorld(FGrid Grid) const
{
	FVector worldPosition;
	worldPosition.X = Grid.X * CellSize;
	worldPosition.Y = Grid.Y * CellSize;

	return worldPosition;
}

template<typename T>
T* FGridSystem<T>::GetValidGridObject(FGrid Grid) const
{
	int32 x = Grid.X;
	int32 y = Grid.Y;

	int32 index = X_Length * x + y;

	if (ObjectArray.IsValidIndex(index))
	{
		T* obj = ObjectArray[index];

		if (IsValid(obj))// && obj->GetGrid() == Grid)
		{
			return obj;
		}
	}

	return nullptr;
}
