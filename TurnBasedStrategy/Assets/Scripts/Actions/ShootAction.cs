using System;
using System.Collections;
using System.Collections.Generic;
using Unity.VisualScripting;
using UnityEngine;
using UnityEngine.EventSystems;

public class ShootAction : BaseAction
{

    public event EventHandler<OnShootEventArgs> onShoot;

    public class OnShootEventArgs : EventArgs
    {
        public Unit targetUnit;
        public Unit shootingUnit;
    }

    private enum State
    { 
    Aiming, Shooting, Cooloff
    }


    private State state;
    [SerializeField]
    private int maxShootDistance = 7;
    private float stateTimer;

    private Unit targetUnit;
    private bool canShootBullet;

    void Update()
    {
        if (!isActive)
        {
            return;
        }

        stateTimer -= Time.deltaTime;

        switch (state)
        {
            case State.Aiming:
                Vector3 aimDir = (targetUnit.GetWorldPosition() - unit.GetWorldPosition()).normalized;

                float rotationSpeed = 10.0f;
                transform.forward = Vector3.Lerp(transform.forward, aimDir, rotationSpeed * Time.deltaTime);

                break;
            case State.Shooting:
                if (canShootBullet)
                {
                    Shoot();
                    canShootBullet = false;
                }
                break;
            case State.Cooloff:

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
            case State.Aiming:
                state = State.Shooting;
                float shootingStateTime = 0.1f;
                stateTimer = shootingStateTime;
                break;
            case State.Shooting:
                state = State.Cooloff;
                float cooloffStateTime = 0.5f;
                stateTimer = cooloffStateTime;
                break;
            case State.Cooloff:
                ActionComplete();
                break;
        }
    }

    public override string GetActionName()
    {
        return "Shoot";
    }

    private void Shoot()
    {
        onShoot?.Invoke(this, new OnShootEventArgs {
            targetUnit = targetUnit, shootingUnit = unit 
        });

        targetUnit.Damage(40);
    }

    public override List<GridPosition> GetValidActionGridPositionList()
    {
        GridPosition unitGridPosition = unit.GetGridPosition();
        return GetValidActionGridPositionList(unitGridPosition);
    }


    public List<GridPosition> GetValidActionGridPositionList(GridPosition unitGridPosition)
    {
        List<GridPosition> validList = new List<GridPosition>();

        for (int x = -maxShootDistance; x <= maxShootDistance; x++)
        {
            for (int z = -maxShootDistance; z <= maxShootDistance; z++)
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
                if (testDistance > maxShootDistance)
                {
                    continue;
                }

                //자기 자신
                if (unitGridPosition == testPosition)
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
        targetUnit = LevelGrid.Instance.GetUnitAtGridPosition(gridPosition);

        state = State.Aiming;
        float aimingStateTime = 1.0f;
        stateTimer = aimingStateTime;

        canShootBullet = true;

        ActionStart(onActionComplete);
    }

    public Unit GetTargetUnit()
    {
        return targetUnit;
    }

    public int GetMaxShootDistance()
    {
        return maxShootDistance;
    }

    public override EnemyAIAction GetEnemyAIAction(GridPosition gridPosition)
    {
        //공격할 적의 HP가 적을 수록 가치가 높다.

        Unit targetUnit = LevelGrid.Instance.GetUnitAtGridPosition(gridPosition);

        return new EnemyAIAction
        {
            gridPosition = gridPosition,
            actionValue = 100 + Mathf.RoundToInt((1 - targetUnit.GetHealthNormalized()) * 100f),
        };
    }

    public int GetTargetCountAtPosition(GridPosition gridPosition)
    {
        return GetValidActionGridPositionList(gridPosition).Count;
    }

}
