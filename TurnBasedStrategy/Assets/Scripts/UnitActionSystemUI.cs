using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
public class UnitActionSystemUI : MonoBehaviour
{

    [SerializeField]
    private Transform actionButtonPrefab; // Prefab에서 ButtonUI 설정 필요.
    [SerializeField]
    private Transform actionBunttonContainerTransnform; // Container 설정 필요.

    private void Start()
    {
        UnitActionSystem.Instance.OnSelectedUnitChanged += UnitActionSystem_OnSelectUnitChanged;
        CreateUnitActionButtons();
    }



    private void CreateUnitActionButtons()
    {
        foreach (Transform buttonTransform in actionBunttonContainerTransnform)
        {
            //주의 : transform은 Destroy할 수 없음.
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
