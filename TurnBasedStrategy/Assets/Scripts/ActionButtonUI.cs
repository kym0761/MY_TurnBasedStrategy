using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using TMPro;
public class ActionButtonUI : MonoBehaviour
{

    //Prefab에서 Set 필요함.
    [SerializeField]
    private TextMeshProUGUI textMeshPro;
    [SerializeField]
    private Button button;

    public void SetBaseAction(BaseAction baseAction)
    {
        textMeshPro.text = baseAction.GetActionName().ToUpper();


        //버튼을 눌렀을 시에 UnitActionSystem의 Action을 바꿈.
        button.onClick.AddListener(() => {
            UnitActionSystem.Instance.SetSelectedAction(baseAction);
        });
    }

}
