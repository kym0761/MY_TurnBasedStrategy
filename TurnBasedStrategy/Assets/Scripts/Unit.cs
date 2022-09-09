using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Unit : MonoBehaviour
{
    //���� ��ġ�� GridPosition
    private GridPosition gridPosition;
    //Move ���
    private MoveAction moveAction;


    private void Awake()
    {
        moveAction = GetComponent<MoveAction>();
    }

    void Start()
    {
        //������ �� ������ ��ġ ����� Grid ������.
        gridPosition = LevelGrid.Instance.GetGridPosition(transform.position);
        LevelGrid.Instance.AddUnitAtGridPosition(gridPosition,this);
    }

    // Update is called once per frame
    void Update()
    {
        //Grid�� ���� Grid�� �ٸ��� LevelGrid�� ���� ������ �ڿ� �ٽ� Set
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
