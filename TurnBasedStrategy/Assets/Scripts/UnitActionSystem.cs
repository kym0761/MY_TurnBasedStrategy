using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;
using UnityEngine.EventSystems;

public class UnitActionSystem : MonoBehaviour
{
    public static UnitActionSystem Instance { get; private set; }


    public event EventHandler OnSelectedUnitChanged;
    public event EventHandler OnSelectedActionChanged;
    public event EventHandler<bool> OnBusyChanged; // args�� bool�� ������ �� ����.
    public event EventHandler OnActionStart;


    [SerializeField] 
    private Unit selectedUnit; // ���õ� ����. ���콺 Ŭ������ ���õ�.
    [SerializeField]
    private LayerMask unitLayerMask; // unit�� �����ؼ� ���̾� Selection�� �����ϰ� �ؾ���.

    private bool isBusy;
    private BaseAction selectedAction;

    private void Awake()
    {
        if (Instance != null)
        {
            Debug.LogError("There's more than one UnitActionSystem! " + transform + " - " + Instance);
            Destroy(gameObject);
            return;
        }

        Instance = this;
    }

    void Start()
    {
        //??
        //SetSelectedUnit(selectedUnit);
    }

    // Update is called once per frame
    void Update()
    {
        if (isBusy)
        {
            return;
        }

        if (!TurnSystem.Instance.IsPlayerTurn())
        {
            return;
        }

        //UI ���� ���콺�� �ö�� �ִ����� Ȯ����.
        if (EventSystem.current.IsPointerOverGameObject())
        {
            return;
        }

        //���� ���ý� ���ָ� �����ϰ� ����.
        //������ �ƴ� �ٴ��� Ŭ���ϸ� GridŬ������ �Ѿ.
        if (TryHandleUnitSelection())
        {
            return;
        }

        HandleSelectedAction();
    }

    private void HandleSelectedAction()
    {
        if (Input.GetMouseButtonDown(0))
        {
            GridPosition mouseGridPosition = LevelGrid.Instance.GetGridPosition(MouseWorld.GetPosition());

            if (!selectedAction)
            {
                return;
            }

            if (!selectedAction.IsValidActionGridPosition(mouseGridPosition))
            {
                return;
            }

            if (!selectedUnit.TrySpendActionPointsToTakeAction(selectedAction))
            {
                return;
            }

            SetBusy();
            selectedAction.TakeAction(mouseGridPosition, ClearBusy);

            OnActionStart?.Invoke(this, EventArgs.Empty);

            ////C# 7.0 Pattern Matching.
            //switch (selectedAction)
            //{
            //    case MoveAction moveAction:

            //        //������ ���� ��, ���콺�� Grid��ġ�� unit���� Valid�ϴٸ� �̵� ����� ����.
            //        if (selectedUnit)
            //        {
            //            bool validMove = moveAction.IsValidActionGridPosition(mouseGridPosition);
            //            if (validMove)
            //            {
            //                SetBusy();
            //                moveAction.Move(mouseGridPosition, ClearBusy);
            //            }
            //        }
            //        break;

            //    case SpinAction spinAction:
            //        SetBusy();
            //        spinAction.Spin(ClearBusy);
            //        break;
            //}
        }
    }

    private bool TryHandleUnitSelection()
    {
        if (Input.GetMouseButtonDown(0))
        {
            Ray ray = Camera.main.ScreenPointToRay(Input.mousePosition);
            RaycastHit raycastHit;
            if (Physics.Raycast(ray, out raycastHit, float.MaxValue, unitLayerMask))
            {
                Unit unit;
                if (raycastHit.transform.TryGetComponent<Unit>(out unit))
                {
                    //unit == selectedUnit means Already Selected that unit.
                    if (unit == selectedUnit)
                    {
                        return false;
                    }

                    if (unit.IsEnemy())
                    {
                        //not select enemy
                        return false;
                    }

                    SetSelectedUnit(unit);
                    return true;
                }

            }
        }


        return false;
    }


    private void SetSelectedUnit(Unit Unit)
    {
        selectedUnit = Unit;
        //MoveAction�� �⺻ Action���� ����.
        SetSelectedAction(Unit.GetMoveAction());


        OnSelectedUnitChanged?.Invoke(this, EventArgs.Empty);

    }

    public void SetSelectedAction(BaseAction baseAction)
    {
        selectedAction = baseAction;



        OnSelectedActionChanged?.Invoke(this, EventArgs.Empty);
    }


    public Unit GetSelectedUnit()
    {
        return selectedUnit;
    }

    public BaseAction GetSelectedAction()
    {
        return selectedAction;
    }

    private void SetBusy()
    {
        isBusy = true;

        OnBusyChanged?.Invoke(this, isBusy);
    }
    private void ClearBusy()
    {
        isBusy = false;

        OnBusyChanged?.Invoke(this, isBusy);
    }
}
