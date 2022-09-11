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

    private Vector3 targetPosition;

    protected override void Awake()
    {
        base.Awake();
        //최초 위치 설정하여 움직이지 않게 막음.
        targetPosition = transform.position;
    }

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

        float stoppingDistance = 0.1f;
        Vector3 moveDirection = (targetPosition - transform.position).normalized;


        //targetPosition으로 이동.
        if (Vector3.Distance(transform.position, targetPosition) > stoppingDistance)
        {

            float moveSpeed = 4.0f;
            transform.position += moveDirection * moveSpeed * Time.deltaTime;

        }
        else
        {
            onStopMoving?.Invoke(this, EventArgs.Empty);
            ActionComplete();
        }

        float rotationSpeed = 10.0f;
        transform.forward = Vector3.Lerp(transform.forward, moveDirection, rotationSpeed * Time.deltaTime);


    }

    public override void TakeAction(GridPosition gridPosition, Action onActionComplete)
    {
        targetPosition = LevelGrid.Instance.GetWorldPosition(gridPosition);

        onStartMoving?.Invoke(this, EventArgs.Empty);

        ActionStart(onActionComplete);
    }


    public override List<GridPosition> GetValidActionGridPositionList()
    {
        //unit의 이동 범위 Grid에서 Valid한 Grid만 걸러냄

        List<GridPosition> validList = new List<GridPosition>();
        GridPosition unitGridPosition = unit.GetGridPosition();

        for (int x = -maxMoveDistance; x <= maxMoveDistance; x++)
        {
            for (int z = -maxMoveDistance; z <= maxMoveDistance; z++)
            {
                //이동량 조절
                int XZ = Mathf.Abs(x) + Mathf.Abs(z);
                if (XZ > maxMoveDistance)
                {
                    continue;
                }
                


                GridPosition offsetposition = new GridPosition(x, z);

                GridPosition testPosition = unitGridPosition + offsetposition;

                //없는 Grid
                if (!LevelGrid.Instance.IsValidGridPosition(testPosition))
                {
                    continue;
                }

                //자기 자신
                if (unitGridPosition == testPosition)
                {
                    continue;
                }

                //누군가 점유
                if (LevelGrid.Instance.HasAnyUnitOnGridPosition(testPosition))
                {
                    continue;
                }

                //통과했다면 문제 없으니 valid에 추가.
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
        //공격 가능한 적이 많을 수록 가치가 높다.

        int targetCountAtGridPosition = unit.GetAction<ShootAction>().GetTargetCountAtPosition(gridPosition);

        return new EnemyAIAction
        {
            gridPosition = gridPosition,
            actionValue = targetCountAtGridPosition * 10,
        };
    }

}
