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
        //어차피 유닛이 많아봤자 수백개일테니 퍼포먼스 고려할 수준은 아님.
        UnitActionSystem.Instance.onSelectedUnitChanged += UnitSelectedVisual_OnSelectedUnitChanged;

        //비활성화를 위함.
        UpdateVisual();
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

    private void OnDestroy()
    {
        //유닛이 죽을 때 Destroy될 것인데, Destroy된 필요없는 비쥬얼은 Unbind할 필요가 있음.
        UnitActionSystem.Instance.onSelectedUnitChanged -= UnitSelectedVisual_OnSelectedUnitChanged;
    }

}
