using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
public class UnitActionSystemUI : MonoBehaviour
{

    [SerializeField]
    private Transform actionButtonPrefab; // Prefab���� ButtonUI ���� �ʿ�.
    [SerializeField]
    private Transform actionBunttonContainerTransnform; // Container ���� �ʿ�.

    private void Start()
    {
        UnitActionSystem.Instance.OnSelectedUnitChanged += UnitActionSystem_OnSelectUnitChanged;
        CreateUnitActionButtons();
    }



    private void CreateUnitActionButtons()
    {
        foreach (Transform buttonTransform in actionBunttonContainerTransnform)
        {
            //���� : transform�� Destroy�� �� ����.
            Destroy(buttonTransform.gameObject);
        }

        Unit selectedunit = UnitActionSystem.Instance.GetSelectedUnit();

        if (!selectedunit)
        {
            return;
        }

        BaseAction[] baseActions = selectedunit.GetBaseActionArray();

        foreach (BaseAction baseAction in baseActions)
        {
           Transform actionButtonTransform = Instantiate(actionButtonPrefab, actionBunttonContainerTransnform);
            ActionButtonUI actionButtonUI = actionButtonTransform.GetComponent<ActionButtonUI>();
            actionButtonUI.SetBaseAction(baseAction);
        
        }

    }

    private void UnitActionSystem_OnSelectUnitChanged(object sender, EventArgs e)
    {
        CreateUnitActionButtons();
    }


}
