// Fill out your copyright notice in the Description page of Project Settings.


#include "GridSystem.h"

template<typename TObject>
FGridSystem<TObject>::FGridSystem(int _X_Length, int _Y_Length, float _CellSize, TFunctionRef<TObject* (const FGridSystem<TObject>,const FGrid)> CreateObjectFunction)
{
	X_Length = _X_Length;
	Y_Length = _Y_Length;
	CellSize = _CellSize;

	
    for (int x = 0; x < X_Length; x++)
    {
        for (int y = 0; y < Y_Length; y++)
        {
            FGrid grid = FGrid(x, z);
            TObject* gridobj = createGridObject(this, gridPosition);

            GridObjectArray.Add(gridobj);
        }
    }

}

