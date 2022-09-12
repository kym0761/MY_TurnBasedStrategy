using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.EventSystems;

public class MoveAction : BaseAction
{

    public event EventHandler onStartMoving;
    public event EventHandler onStopMoving;

 
    [SerializeField]
    private int maxMoveDistance = 4;

    private List<Vector3> positionList;
    int currentPositionIndex;


    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        if (!isActive)
        {
            return;
        }

        Vector3 targetPosition = positionList[currentPositionIndex];
        Vector3 moveDirection = (targetPosition - transform.position).normalized;

        float rotationSpeed = 10.0f;
        transform.forward = Vector3.Lerp(transform.forward, moveDirection, rotationSpeed * Time.deltaTime);


        float stoppingDistance = 0.1f;
        //targetPosition���� �̵�.
        if (Vector3.Distance(transform.position, targetPosition) > stoppingDistance)
        {

            float moveSpeed = 4.0f;
            transform.position += moveDirection * moveSpeed * Time.deltaTime;

        }
        else
        {
            currentPositionIndex++;

            if (currentPositionIndex >= positionList.Count)
            {
                onStopMoving?.Invoke(this, EventArgs.Empty);
                ActionComplete();
            }
        }

    }

    public override void TakeAction(GridPosition gridPosition, Action onActionComplete)
    {
        List<GridPosition> pathList = PathFinding.Instance.FindPath(unit.GetGridPosition(), gridPosition, out int pathLength);


        currentPositionIndex = 0;
        positionList = new List<Vector3>();
        //positionList.Add(LevelGrid.Instance.GetWorldPosition(gridPosition));

        foreach (GridPosition pathGridPosition in pathList)
        {
            positionList.Add(LevelGrid.Instance.GetWorldPosition(pathGridPosition));
        }


        onStartMoving?.Invoke(this, EventArgs.Empty);

        ActionStart(onActionComplete);
    }


    public override List<GridPosition> GetValidActionGridPositionList()
    {
        //unit�� �̵� ���� Grid���� Valid�� Grid�� �ɷ���

        List<GridPosition> validList = new List<GridPosition>();
        GridPosition unitGridPosition = unit.GetGridPosition();

        for (int x = -maxMoveDistance; x <= maxMoveDistance; x++)
        {
            for (int z = -maxMoveDistance; z <= maxMoveDistance; z++)
            {
                ////�̵��� ����
                //int XZ = Mathf.Abs(x) + Mathf.Abs(z);
                //if (XZ > maxMoveDistance)
                //{
                //    continue;
                //}
                


                GridPosition offsetposition = new GridPosition(x, z);

                GridPosition testPosition = unitGridPosition + offsetposition;

                //���� Grid
                if (!LevelGrid.Instance.IsValidGridPosition(testPosition))
                {
                    continue;
                }

                //�ڱ� �ڽ�
                if (unitGridPosition == testPosition)
                {
                    continue;
                }

                //������ ����
                if (LevelGrid.Instance.HasAnyUnitOnGridPosition(testPosition))
                {
                    continue;
                }

                //���� �� �ִ� ��ġ����
                if (!PathFinding.Instance.IsWalkableGridPosition(testPosition))
                {
                    continue;
                }

                //���� ������ ��ġ����
                if (!PathFinding.Instance.HasPath(unitGridPosition, testPosition))
                {
                    continue;
                }

                int multiplier = 10;
                //�ǵ��� �޸� �Ÿ��� �� ��ġ
                if (PathFinding.Instance.GetPathLength(unitGridPosition, testPosition) > maxMoveDistance * multiplier)
                {
                    continue;
                }


                //����ߴٸ� ���� ������ valid�� �߰�.
                validList.Add(testPosition);
            }
        }

        return validList;
    }

    public override string GetActionName()
    {
        return "Move";
    }

    public override EnemyAIAction GetEnemyAIAction(GridPosition gridPosition)
    {
        //���� ������ ���� ���� ���� ��ġ�� ����.

        int targetCountAtGridPosition = unit.GetAction<ShootAction>().GetTargetCountAtPosition(gridPosition);

        return new EnemyAIAction
        {
            gridPosition = gridPosition,
            actionValue = targetCountAtGridPosition * 10,
        };
    }

}
