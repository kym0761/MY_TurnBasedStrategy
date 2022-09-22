using System;
using System.Collections;
using System.Collections.Generic;
using UnityEditor.Rendering.Universal;
using UnityEngine;

public class GridSystem<TGridObject>
{
    //grid Setting 정보
    private int width;
    private int height;
    private float cellSize;

    //실제 grid 정보 보관. width, height 만큼 미리 확보함.
    private TGridObject[,] gridObjectArray;


    //Func<GridSystem<TGridObject>, GridPosition, TGridObject> createGridObject = generic
    public GridSystem(int Width, int Height, float CellSize, Func<GridSystem<TGridObject>, GridPosition, TGridObject> createGridObject)
    {
        width = Width;
        height = Height;
        cellSize = CellSize;

        gridObjectArray = new TGridObject[Width, Height];

        for (int x = 0; x < Width; x++)
        {
            for (int z = 0; z < Height; z++)
            {
                GridPosition gridPosition = new GridPosition(x, z);
                TGridObject gridobj = createGridObject(this,gridPosition);
                gridObjectArray[x, z] = gridobj;
            }
        }

    }

    public Vector3 GetWorldPosition(GridPosition gridPosition)
    {
        //Grid의 World Position.

        return new Vector3(gridPosition.x, 0, gridPosition.z) * cellSize;
    }

    public GridPosition GetGridPosition(Vector3 WorldPosition)
    {
        //World position에 해당하는 GridPosition

        return new GridPosition(
            Mathf.RoundToInt(WorldPosition.x / cellSize), 
            Mathf.RoundToInt(WorldPosition.z / cellSize));
    }

    public void CreateDebugObjects(Transform debugPrefab)
    {
        for (int x = 0; x < width; x++)
        {
            for (int z = 0; z < height; z++)
            {
                GridPosition gridPosition = new GridPosition(x, z);
                Transform debugTransform = GameObject.Instantiate(debugPrefab, GetWorldPosition(gridPosition), Quaternion.identity);

                GridDebugObject gridDebugObject = debugTransform.GetComponent<GridDebugObject>();
                gridDebugObject.SetGridObject(GetGridObject(gridPosition));
            }
        }
    }

    public TGridObject GetGridObject(GridPosition gridPosition)
    {
        return gridObjectArray[gridPosition.x, gridPosition.z];
    }

    public bool IsValidGridPosition(GridPosition gridPosition)
    {
        return gridPosition.x >= 0 && 
            gridPosition.z >= 0 && 
            gridPosition.x < width &&
            gridPosition.z < height;
    }

    public int GetWidth()
    {
        return width;
    }

    public int GetHeight()
    {
        return height;
    }

}
