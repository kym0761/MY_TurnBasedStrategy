using System.Collections;
using System.Collections.Generic;
using UnityEditor.Rendering.Universal;
using UnityEngine;

public class GridSystem
{
    private int width;
    private int height;
    private float cellSize;
    private GridObject[,] gridObjectArray;


    public GridSystem(int Width, int Height, float CellSize)
    {
        width = Width;
        height = Height;
        cellSize = CellSize;

        gridObjectArray = new GridObject[Width, Height];

        for (int x = 0; x < Width; x++)
        {
            for (int z = 0; z < Height; z++)
            {
                GridObject gridobj = new GridObject(this, new GridPosition(x, z));
                gridObjectArray[x, z] = gridobj;
                
                //Debug.DrawLine(GetWorldPosition(x,z), GetWorldPosition(x,z) + Vector3.right*0.2f,Color.white, 1000);
            }
        }

    }

    public Vector3 GetWorldPosition(GridPosition gridPosition)
    {
        return new Vector3(gridPosition.X, 0, gridPosition.Z) * cellSize;
    }

    public GridPosition GetGridPosition(Vector3 WorldPosition)
    {
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

    public GridObject GetGridObject(GridPosition gridPosition)
    {
        return gridObjectArray[gridPosition.X, gridPosition.Z];
    }

    public bool IsValidGridPosition(GridPosition gridPosition)
    {
        return gridPosition.X >= 0 && 
            gridPosition.Z >= 0 && 
            gridPosition.X < width &&
            gridPosition.Z < height;
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
