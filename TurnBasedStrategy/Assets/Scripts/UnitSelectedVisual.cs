using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UnitSelectedVisual : MonoBehaviour
{

    //유닛에게 붙는 선택됨을 알리는 외형

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
        //UnitActionSystem의 델리게이트에 비쥬얼 토글 켬.
        //클릭마다 모든 유닛들이 이 행동을 하게 되는게 비효율적이긴 하지만, 간단함.
        UnitActionSystem.Instance.OnSelectedUnitChanged += UnitSelectedVisual_OnSelectedUnitChanged;

        //비활성화를 위함.
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
