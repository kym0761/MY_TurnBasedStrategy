// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Grid.h"
#include "GridObject.h"
/**
 * https://docs.unrealengine.com/5.0/en-US/API/Runtime/Core/GenericPlatform/TFunction/
 */

template <typename TObject>
class TURNBASESTRATEGY_API FGridSystem : public UObject
{

private:

	int32 X_Length = 10;
	int32 Y_Length = 10;
	float CellSize = 100.0f;

	TArray<TObject*> GridObjectArray;

public:

	FGridSystem(int _X_Length, int _Y_Length, float _CellSize, TFunctionRef<TObject*(const FGridSystem<TObject>, const FGrid)> CreateObjectFunction);
};


