using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SwordAction : BaseAction
{
    public static event EventHandler onAnySwordHit;

    public event EventHandler onSwordActionStarted;
    public event EventHandler onSwordActionComplete;

    private int maxSwordDistance = 1;
    private State state;
    private float stateTimer;
    private Unit targetUnit;

    private enum State
    {
        SwingingBeforeHit,
        SwingingAfterHit,
    }

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

                //���� Grid
                if (!LevelGrid.Instance.IsValidGridPosition(testPosition))
                {
                    continue;
                }

                //������ �������� ���� ���� ���� ���� �Ұ�
                if (!LevelGrid.Instance.HasAnyUnitOnGridPosition(testPosition))
                {
                    continue;
                }

                Unit targetUnit = LevelGrid.Instance.GetUnitAtGridPosition(testPosition);

                //���� ���� Skip.
                if (targetUnit.IsEnemy() == unit.IsEnemy())
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
        targetUnit = LevelGrid.Instance.GetUnitAtGridPosition(gridPosition);

        state = State.SwingingBeforeHit;
        float beforeHitStateTime = 0.7f;
        stateTimer = beforeHitStateTime;

        onSwordActionStarted?.Invoke(this, EventArgs.Empty);

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

        //ActionComplete();

        stateTimer -= Time.deltaTime;

        switch (state)
        {
            case State.SwingingBeforeHit:
                Vector3 aimDir = (targetUnit.GetWorldPosition() - unit.GetWorldPosition()).normalized;

                float rotationSpeed = 10.0f;
                transform.forward = Vector3.Lerp(transform.forward, aimDir, rotationSpeed * Time.deltaTime);

                break;
            case State.SwingingAfterHit:

                break;


        }

        if (stateTimer <= 0.0f)
        {
            NextState();
        }

    }

    private void NextState()
    {
        switch (state)
        {
            case State.SwingingBeforeHit:
                state = State.SwingingAfterHit;
                float afterHitStateTime = 0.5f;
                stateTimer = afterHitStateTime;
                targetUnit.Damage(100);
                onAnySwordHit?.Invoke(this,EventArgs.Empty);
                break;
            case State.SwingingAfterHit:
                onSwordActionComplete?.Invoke(this, EventArgs.Empty);
                ActionComplete();
                break;
        }
    }

    public int GetMaxSwordDistance()
    {
        return maxSwordDistance;
    }


}
