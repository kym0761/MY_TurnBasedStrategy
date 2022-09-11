using System;
using System.Collections;
using System.Collections.Generic;
using Unity.VisualScripting;
using UnityEngine;

public class Unit : MonoBehaviour
{
    private const int ACTION_POINTS_MAX = 2;

    public static event EventHandler onAnyActionPointsChanged;
    public static event EventHandler onAnyUnitSpawned;
    public static event EventHandler onAnyUnitDead;

    [SerializeField] private bool isEnemy;


    //현재 위치의 GridPosition
    private GridPosition gridPosition;
    //Move 담당
    private MoveAction moveAction;
    private SpinAction spinAction;

    private BaseAction[] baseActionArray;
    private HealthSystem healthSystem;

    private int actionPoints = ACTION_POINTS_MAX;

    private void Awake()
    {
        moveAction = GetComponent<MoveAction>();
        spinAction = GetComponent<SpinAction>();
        healthSystem = GetComponent<HealthSystem>();
        baseActionArray = GetComponents<BaseAction>();

    }

    void Start()
    {
        //시작할 때 기준의 위치 값대로 Grid 설정함.
        gridPosition = LevelGrid.Instance.GetGridPosition(transform.position);
        LevelGrid.Instance.AddUnitAtGridPosition(gridPosition,this);

        TurnSystem.Instance.onTurnChanged += TurnSystem_onTurnChanged;
        healthSystem.onDead += HealthSystem_OnDead;

        onAnyUnitSpawned?.Invoke(this, EventArgs.Empty);
    }

    // Update is called once per frame
    void Update()
    {
        //Grid가 현재 Grid와 다르면 LevelGrid의 값을 변경한 뒤에 다시 Set
        GridPosition newGridPosition = LevelGrid.Instance.GetGridPosition(transform.position);
        if (newGridPosition != gridPosition)
        {
            GridPosition oldGridPosition = gridPosition;
            gridPosition = newGridPosition;

            LevelGrid.Instance.UnitMovedGridPosition(this, oldGridPosition, newGridPosition);
        }
    }

    public MoveAction GetMoveAction()
    {
        return moveAction;
    }

    public SpinAction GetSpinAction()
    {
        return spinAction;
    }

    public GridPosition GetGridPosition()
    {
        return gridPosition;
    }

    public BaseAction[] GetBaseActionArray()
    {
        return baseActionArray;
    }

    public bool TrySpendActionPointsToTakeAction(BaseAction baseAction)
    {
        if (CanSpendActionPointsToTakeAction(baseAction))
        {
            SpendActionPoints(baseAction.GetActionPointsCost());
            return true;
        }
        else
        {
            return false;
        }

    }

    public bool CanSpendActionPointsToTakeAction(BaseAction baseAction)
    {
        return actionPoints >= baseAction.GetActionPointsCost();
    }

    private void SpendActionPoints(int amount)
    {
        actionPoints -= amount;

        onAnyActionPointsChanged?.Invoke(this, EventArgs.Empty);
    }

    public int GetActionPoints()
    {
        return actionPoints;
    }


    private void TurnSystem_onTurnChanged(object sender, EventArgs e)
    {
        if ((IsEnemy() && !TurnSystem.Instance.IsPlayerTurn()) || (!IsEnemy() && TurnSystem.Instance.IsPlayerTurn()))
        {
            actionPoints = ACTION_POINTS_MAX;

            onAnyActionPointsChanged?.Invoke(this, EventArgs.Empty);
        }
    }

    private void HealthSystem_OnDead(object sender, EventArgs e)
    {
        LevelGrid.Instance.RemoveUnitAtGridPosition(gridPosition, this);

        Destroy(gameObject);

        onAnyUnitDead?.Invoke(this, EventArgs.Empty);
    }

    public bool IsEnemy()
    {
        return isEnemy;
    }

    public void Damage(int damageAmount)
    {
        healthSystem.Damage(damageAmount);
        //Debug.Log(transform+" Damaged");
    }

    public Vector3 GetWorldPosition()
    {
        return transform.position;
    }
}
