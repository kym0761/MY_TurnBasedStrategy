using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class InteractAction : BaseAction
{
    private int maxInteractDistance = 1;

    private void Update()
    {
        if (!isActive)
        {
            return;
        }
    }

    public override string GetActionName()
    {
        return "Interact";
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

        for (int x = -maxInteractDistance; x <= maxInteractDistance; x++)
        {
            for (int z = -maxInteractDistance; z <= maxInteractDistance; z++)
            {
                GridPosition offsetposition = new GridPosition(x, z);

                GridPosition testPosition = unitGridPosition + offsetposition;

                //���� Grid
                if (!LevelGrid.Instance.IsValidGridPosition(testPosition))
                {
                    continue;
                }


                Door door = LevelGrid.Instance.GetDoorAtGridPosition(testPosition);
                if (door == null)
                {
                    continue;
                }

                //����ߴٸ� ���� ������ valid�� �߰�.
                validList.Add(testPosition);
            }
        }

        return validList;
    }

    public override void TakeAction(GridPosition gridPosition, Action onActionComplete)
    {
        Door door = LevelGrid.Instance.GetDoorAtGridPosition(gridPosition);
        door.Interact(OnInteractComplete);

        ActionStart(onActionComplete);
    }

    private void OnInteractComplete()
    {
        ActionComplete();
    }

}
