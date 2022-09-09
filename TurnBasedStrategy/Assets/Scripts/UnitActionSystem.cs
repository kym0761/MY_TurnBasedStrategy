using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;

public class UnitActionSystem : MonoBehaviour
{
    public static UnitActionSystem Instance { get; private set; }


    public event EventHandler OnSelectedUnitChanged;
    [SerializeField] 
    private Unit _SelectedUnit; // 선택된 유닛. 마우스 클릭으로 선택됨.
    [SerializeField]
    private LayerMask _UnitLayerMask; //선택 가능한 unit의 레이어 마스크를 선택해줘야함.

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
        
    }

    // Update is called once per frame
    void Update()
    {

        if (Input.GetMouseButtonDown(0))
        {
            //유닛 선택시 유닛만 선택하고 끝남.
            //유닛이 아닌 바닥을 클릭하면 Grid클릭으로 넘어감.
            if (TryHandleUnitSelection())
            {
                return;
            }

            GridPosition mouseGridPosition = LevelGrid.Instance.GetGridPosition(MouseWorld.GetPosition());

            //유닛이 있을 때, 마우스의 Grid위치가 unit에게 Valid하다면 이동 명령을 내림.
            if (_SelectedUnit)
            {
                bool validMove = _SelectedUnit.GetMoveAction().IsValidActionGridPosition(mouseGridPosition);
                if (validMove)
                {
                    _SelectedUnit.GetMoveAction().Move(mouseGridPosition);
                }
            }
        }
    }


    private bool TryHandleUnitSelection()
    {
        Ray ray = Camera.main.ScreenPointToRay(Input.mousePosition);
        RaycastHit raycastHit;
        if (Physics.Raycast(ray, out raycastHit, float.MaxValue, _UnitLayerMask))
        {
            Unit unit;
            if (raycastHit.transform.TryGetComponent<Unit>(out unit))
            {
                SetSelectedUnit(unit);
                return true;
            }

        }

        return false;
    }


    private void SetSelectedUnit(Unit Unit)
    {
        _SelectedUnit = Unit;
        OnSelectedUnitChanged?.Invoke(this, EventArgs.Empty);

    }

    public Unit GetSelectedUnit()
    {
        return _SelectedUnit;
    }

}
