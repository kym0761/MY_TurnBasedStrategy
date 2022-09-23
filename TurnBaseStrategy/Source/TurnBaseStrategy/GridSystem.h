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

	FGridSystem()
	{
		X_Length = 10;
		Y_Length = 10;
		CellSize = 100.0f;
	}
	~FGridSystem()
	{
		//UE_LOG(LogTemp, Warning, TEXT("~FGridSystem Called"));
		ObjectArray.Empty();
	}

	FGridSystem(int _X_Length, int _Y_Length, float _CellSize,
		TFunctionRef<T* (FGridSystem<T>*, FGrid)> CreateObjectFunction)
	{
		X_Length = _X_Length;
		Y_Length = _Y_Length;
		CellSize = _CellSize;


		for (int x = 0; x < X_Length; x++)
		{
			for (int y = 0; y < Y_Length; y++)
			{
				FGrid grid = FGrid(x, y);
				T* gridobj = CreateObjectFunction(this, grid);
				gridobj->SetGrid(grid);
				ObjectArray.Add(gridobj);
			}
		}
	}

	TArray<T*> GetObjectArray() const;

	FGrid WorldToGrid(FVector WorldPosition);
	FVector GridToWorld(FGrid Grid);

	T* GetValidGridObject(FGrid Grid);
};

//template <typename T, TEnableIf< TIsDerivedFrom<T, UObject>::IsDerivedType>::Type>
//FGridSystem<T>::FGridSystem()
//{
//
//}

//template <typename T, TEnableIf< TIsDerivedFrom<T, UObject>::IsDerivedType>::Type>
//FGridSystem<T>::~FGridSystem()
//{
//	UE_LOG(LogTemp, Warning, TEXT("~FGridSystem Called"));
//	GridObjectArray.Empty();
//}

//template <typename T, TEnableIf< TIsDerivedFrom<T, UObject>::IsDerivedType>::Type>
//FGridSystem<T>::FGridSystem(int _X_Length, int _Y_Length, float _CellSize,
//	TFunctionRef<T* (FGridSystem<T>*, FGrid)> CreateObjectFunction)
//{
//	X_Length = _X_Length;
//	Y_Length = _Y_Length;
//	CellSize = _CellSize;
//
//
//	for (int x = 0; x < X_Length; x++)
//	{
//		for (int y = 0; y < Y_Length; y++)
//		{
//			FGrid grid = FGrid(x, y);
//			UGridObject* gridobj = CreateObjectFunction(this, grid);
//			gridobj->SetGrid(grid);
//			GridObjectArray.Add(gridobj);
//		}
//	}
//}

template<typename T>
inline TArray<T*> FGridSystem<T>::GetObjectArray() const
{
	return ObjectArray;
}

template<typename T>
inline FGrid FGridSystem<T>::WorldToGrid(FVector WorldPosition)
{
	FGrid grid;
	grid.X = FMath::RoundToInt(WorldPosition.X / CellSize);
	grid.Y = FMath::RoundToInt(WorldPosition.Y / CellSize);

	return grid;
}

template<typename T>
inline FVector FGridSystem<T>::GridToWorld(FGrid Grid)
{
	FVector worldPosition;
	worldPosition.X = Grid.X * CellSize;
	worldPosition.Y = Grid.Y * CellSize;

	return worldPosition;
}

template<typename T>
T* FGridSystem<T>::GetValidGridObject(FGrid Grid)
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
