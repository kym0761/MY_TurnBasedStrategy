using System;
using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;
using UnityEngine.UI;

public class TurnSystemUI : MonoBehaviour
{
    [SerializeField]
    private Button endTurnBtn;
    [SerializeField]
    private TextMeshProUGUI turnNumberText;


    private void Start()
    {
        endTurnBtn.onClick.AddListener(() => {
            TurnSystem.Instance.NextTurn(); 
        });

        TurnSystem.Instance.onTurnChanged += TurnSystem_onTurnChanged;

        UpdateTurnText();
    }

    private void TurnSystem_onTurnChanged(object sender, EventArgs e)
    {
        UpdateTurnText();
    }

    private void UpdateTurnText()
    {
        turnNumberText.text = "Turn : " + TurnSystem.Instance.GetTurnNumber();
    }
}
