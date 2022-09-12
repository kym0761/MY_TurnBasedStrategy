using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SwordAction : BaseAction
{
    private int maxSwordDistance = 1;


    public override string GetActionName()
    {
        return "Sword";
    }

    public override EnemyAIAction GetEnemyAIAction(GridPosition gridPosition)
    {
        return new EnemyAIAction
        {
            gridPosition = gridPosition,
            actionValue = 200,
        };
    }

    public override List<GridPosition> GetValidActionGridPositionList()
    {
        List<GridPosition> validList = new List<GridPosition>();

        GridPosition unitGridPosition = unit.GetGridPosition();

        for (int x = -maxSwordDistance; x <= maxSwordDistance; x++)
        {
            for (int z = -maxSwordDistance; z <= maxSwordDistance; z++)
            {
                GridPosition offsetposition = new GridPosition(x, z);

                GridPosition testPosition = unitGridPosition + offsetposition;

                //없는 Grid
                if (!LevelGrid.Instance.IsValidGridPosition(testPosition))
                {
                    continue;
                }

                //누군가 점유하지 않은 곳은 공격 선택 불가
                if (!LevelGrid.Instance.HasAnyUnitOnGridPosition(testPosition))
                {
                    continue;
                }

                Unit targetUnit = LevelGrid.Instance.GetUnitAtGridPosition(testPosition);

                //같은 팀은 Skip.
                if (targetUnit.IsEnemy() == unit.IsEnemy())
                {
                    continue;
                }

                //통과했다면 문제 없으니 valid에 추가.
                validList.Add(testPosition);
            }
        }

        return validList;
    }

    public override void TakeAction(GridPosition gridPosition, Action onActionComplete)
    {

        ActionStart(onActionComplete);

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

        ActionComplete();

    }

    public int GetMaxSwordDistance()
    {
        return maxSwordDistance;
    }


}
