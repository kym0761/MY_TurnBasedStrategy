using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;

public class UnitActionSystem : MonoBehaviour
{
    public static UnitActionSystem Instance { get; private set; }


    public event EventHandler OnSelectedUnitChanged;
    [SerializeField] 
    private Unit _SelectedUnit;
    [SerializeField]
    private LayerMask _UnitLayerMask;

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
            if (TryHandleUnitSelection())
            {
                return;
            }

            if (_SelectedUnit)
            {
                _SelectedUnit.Move(MouseWorld.GetPosition());
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
