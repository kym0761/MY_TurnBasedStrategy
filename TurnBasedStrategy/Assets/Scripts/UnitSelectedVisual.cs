using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UnitSelectedVisual : MonoBehaviour
{

    //���ֿ��� �ٴ� ���õ��� �˸��� ����

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
        //UnitActionSystem�� ��������Ʈ�� ����� ��� ��.
        //Ŭ������ ��� ���ֵ��� �� �ൿ�� �ϰ� �Ǵ°� ��ȿ�����̱� ������, ������.
        //������ ������ ���ƺ��� ���鰳���״� �����ս� ����� ������ �ƴ�.
        UnitActionSystem.Instance.onSelectedUnitChanged += UnitSelectedVisual_OnSelectedUnitChanged;

        //��Ȱ��ȭ�� ����.
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
        //������ ���� �� Destroy�� ���ε�, Destroy�� �ʿ���� ������� Unbind�� �ʿ䰡 ����.
        UnitActionSystem.Instance.onSelectedUnitChanged -= UnitSelectedVisual_OnSelectedUnitChanged;
    }

}
