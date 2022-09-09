using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Unit : MonoBehaviour
{
    //현재 위치의 GridPosition
    private GridPosition gridPosition;
    //Move 담당
    private MoveAction moveAction;


    private void Awake()
    {
        moveAction = GetComponent<MoveAction>();
    }

    void Start()
    {
        //시작할 때 기준의 위치 값대로 Grid 설정함.
        gridPosition = LevelGrid.Instance.GetGridPosition(transform.position);
        LevelGrid.Instance.AddUnitAtGridPosition(gridPosition,this);
    }

    // Update is called once per frame
    void Update()
    {
        //Grid가 현재 Grid와 다르면 LevelGrid의 값을 변경한 뒤에 다시 Set
        GridPosition newGridPosition = LevelGrid.Instance.GetGridPosition(transform.position);
        if (newGridPosition != gridPosition)
        {
            LevelGrid.Instance.UnitMovedGridPosition(this, gridPosition, newGridPosition);
            gridPosition = newGridPosition;
        }
    }

    public MoveAction GetMoveAction()
    {
        return moveAction;
    }

    public GridPosition GetGridPosition()
    {
        return gridPosition;
    }

}
