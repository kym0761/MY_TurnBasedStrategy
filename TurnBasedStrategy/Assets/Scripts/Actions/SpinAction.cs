using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;

public class SpinAction : BaseAction
{
    private float totalSpinAmount;

    protected override void Awake()
    {
        base.Awake();
    }


    void Update()
    {
        if (!isActive)
        {
            return;
        }

        float spinAddAmount = 360.0f * Time.deltaTime;
        transform.eulerAngles += new Vector3(0, spinAddAmount, 0);
        totalSpinAmount += spinAddAmount;

        if (totalSpinAmount >= 360.0f)
        {
            isActive = false;
            onActionComplete();
        }

    }

    public override void TakeAction(GridPosition gridPosition, Action spinComplete)
    {
        onActionComplete = spinComplete;
        isActive = true;
        totalSpinAmount = 0;
    }

    public override string GetActionName()
    {
        return "Spin";
    }

    public override List<GridPosition> GetValidActionGridPositionList()
    {
        GridPosition unitGridPosition = unit.GetGridPosition();

        return new List<GridPosition> { unitGridPosition };

    }

    public override int GetActionPointsCost()
    {
        return 2;
    }

}
