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
    public event EventHandler<bool> OnBusyChanged; // args로 bool을 전달할 수 있음.
    public event EventHandler OnActionStart;


    [SerializeField] 
    private Unit selectedUnit; // 선택된 유닛. 마우스 클릭으로 선택됨.
    [SerializeField]
    private LayerMask unitLayerMask; // unit을 선택해서 레이어 Selection이 가능하게 해야함.

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

        //UI 위에 마우스가 올라와 있는지를 확인함.
        if (EventSystem.current.IsPointerOverGameObject())
        {
            return;
        }

        //유닛 선택시 유닛만 선택하고 끝남.
        //유닛이 아닌 바닥을 클릭하면 Grid클릭으로 넘어감.
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

            //        //유닛이 있을 때, 마우스의 Grid위치가 unit에게 Valid하다면 이동 명령을 내림.
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
        //MoveAction을 기본 Action으로 세팅.
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
