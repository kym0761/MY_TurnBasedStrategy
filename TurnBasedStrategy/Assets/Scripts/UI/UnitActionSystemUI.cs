using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using TMPro;

public class UnitActionSystemUI : MonoBehaviour
{

    [SerializeField]
    private Transform actionButtonPrefab; // Prefab에서 ButtonUI 설정 필요.
    [SerializeField]
    private Transform actionBunttonContainerTransnform; //Prefab에서 Container 설정 필요.
    [SerializeField]
    private TextMeshProUGUI actionPointsText; //Prefab에서 Text 설정 필요.


    private List<ActionButtonUI> actionButtonUIList;

    private void Awake()
    {
        actionButtonUIList = new List<ActionButtonUI>();
    }

    private void Start()
    {
        UnitActionSystem.Instance.OnSelectedUnitChanged += UnitActionSystem_OnSelectUnitChanged;
        UnitActionSystem.Instance.OnSelectedActionChanged += UnitActionSystem_OnSelectActionChanged;
        UnitActionSystem.Instance.OnActionStart += UnitActionSystem_OnActionStart;
        TurnSystem.Instance.onTurnChanged += TurnSystem_onTurnChanged;
        Unit.OnAnyActionPointsChanged += Unit_OnAnyActionPointsChanged;

        UpdateActionPoints();
        CreateUnitActionButtons();
        UpdateSelectedVisual();
    }



    private void CreateUnitActionButtons()
    {
        foreach (Transform buttonTransform in actionBunttonContainerTransnform)
        {
            //주의 : transform은 Destroy할 수 없음.
            Destroy(buttonTransform.gameObject);
        }

        actionButtonUIList.Clear();



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

            actionButtonUIList.Add(actionButtonUI);
        }

    }

    private void UnitActionSystem_OnSelectUnitChanged(object sender, EventArgs e)
    {
        CreateUnitActionButtons();
        UpdateSelectedVisual();
        UpdateActionPoints();
    }

    private void UnitActionSystem_OnSelectActionChanged(object sender, EventArgs e)
    {
        UpdateSelectedVisual();
    }

    private void TurnSystem_onTurnChanged(object sender, EventArgs e)
    {
        UpdateActionPoints();
    }

    private void UnitActionSystem_OnActionStart(object sender, EventArgs e)
    {
        UpdateActionPoints();
    }

    private void Unit_OnAnyActionPointsChanged(object sender, EventArgs e)
    {
        UpdateActionPoints();
    }

    private void UpdateSelectedVisual()
    {
        foreach (ActionButtonUI actionButtonUI in actionButtonUIList)
        {
            actionButtonUI.UpdateSelectedVisual();
        }
    }

    private void UpdateActionPoints()
    {
        Unit selectedUnit = UnitActionSystem.Instance.GetSelectedUnit();

        actionPointsText.text = "Action Points : " + selectedUnit.GetActionPoints();
    }

}
