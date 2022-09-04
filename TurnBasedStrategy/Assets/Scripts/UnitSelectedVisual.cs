using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UnitSelectedVisual : MonoBehaviour
{

    [SerializeField]
    private Unit _Unit;

    [SerializeField]
    private MeshRenderer _MeshRenderer;
    private void Awake()
    {
        _MeshRenderer = GetComponent<MeshRenderer>();
    }

    // Start is called before the first frame update
    void Start()
    {
        UnitActionSystem.Instance.OnSelectedUnitChanged += UnitSelectedVisual_OnSelectedUnitChanged;

        UpdateVisual();
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    private void UnitSelectedVisual_OnSelectedUnitChanged(object sender, EventArgs empty)
    {
        UpdateVisual();
    }


    private void UpdateVisual()
    {
        if (UnitActionSystem.Instance.GetSelectedUnit() == _Unit)
        {
            _MeshRenderer.enabled = true;
        }
        else
        {
            _MeshRenderer.enabled = false;
        }
    }
}
