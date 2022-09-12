using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class GrenadeAction : BaseAction
{
    [SerializeField]
    private int maxThrowDistance = 7;
    [SerializeField] 
    private Transform grenadeProjectilePrefab;

    private void Update()
    {
        if (!isActive)
        {
            return;
        }

        ActionComplete();
    }

    public override string GetActionName()
    {
        return "Grenade";
    }

    public override EnemyAIAction GetEnemyAIAction(GridPosition gridPosition)
    {
        return new EnemyAIAction
        {
            gridPosition = gridPosition,
            actionValue = 0,
        };
    }

    public override List<GridPosition> GetValidActionGridPositionList()
    {
        List<GridPosition> validList = new List<GridPosition>();

        GridPosition unitGridPosition = unit.GetGridPosition();

        for (int x = -maxThrowDistance; x <= maxThrowDistance; x++)
        {
            for (int z = -maxThrowDistance; z <= maxThrowDistance; z++)
            {
                GridPosition offsetposition = new GridPosition(x, z);

                GridPosition testPosition = unitGridPosition + offsetposition;

                //없는 Grid
                if (!LevelGrid.Instance.IsValidGridPosition(testPosition))
                {
                    continue;
                }

                //사거리 체크
                int testDistance = Mathf.Abs(x) + Mathf.Abs(z);
                if (testDistance > maxThrowDistance)
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
        Transform grenadeProjectileTransform = Instantiate(grenadeProjectilePrefab, unit.GetWorldPosition(), Quaternion.identity);
        GrenadeProjectile grenadeProjectile = grenadeProjectileTransform.GetComponent<GrenadeProjectile>();
        grenadeProjectile.Setup(gridPosition, OnGrenadeBehaviourComplete);


        ActionStart(onActionComplete);
    }

    private void OnGrenadeBehaviourComplete()
    {
        ActionComplete();
    }

}
