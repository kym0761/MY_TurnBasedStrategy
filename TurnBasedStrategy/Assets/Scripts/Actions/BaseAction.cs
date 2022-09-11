using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;
public abstract class BaseAction : MonoBehaviour
{
    public static event EventHandler onAnyActionStarted;
    public static event EventHandler onAnyActionCompleted;

    protected Unit unit;
    protected bool isActive;
    protected Action onActionComplete;

    //private Action onSpinComplete;
    //using System; 필요함.
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
        //ValidGrid인지 확인.
        return GetValidActionGridPositionList().Contains(gridPosition);
    }
    public abstract List<GridPosition> GetValidActionGridPositionList();

    public virtual int GetActionPointsCost()
    {
        return 1;
    }

    protected void ActionStart(Action onActionComplete)
    {
        isActive = true;
        this.onActionComplete = onActionComplete;

        onAnyActionStarted?.Invoke(this, EventArgs.Empty);
    }

    protected void ActionComplete()
    {
        isActive = false;
        onActionComplete();

        onAnyActionCompleted?.Invoke(this, EventArgs.Empty);
    }

    public Unit GetUnit()
    {
        return unit;
    }

}
