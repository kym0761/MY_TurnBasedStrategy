using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;
public abstract class BaseAction : MonoBehaviour
{

    protected Unit unit;
    protected bool isActive;
    protected Action onActionComplete;

    //private Action onSpinComplete;
    //using System; �ʿ���.
    //private delegate void onSomething();
    //private Action<int,bool> a;
    //private Func<string,int> b;

    protected virtual void Awake()
    {
        unit = GetComponent<Unit>();
    }

    public abstract string GetActionName();

    public abstract void TakeAction(GridPosition gridPosition, Action onActionComplete);

    public virtual bool IsValidActionGridPosition(GridPosition gridPosition)
    {
        //ValidGrid���� Ȯ��.

        return GetValidActionGridPositionList().Contains(gridPosition);
    }
    public abstract List<GridPosition> GetValidActionGridPositionList();
}
