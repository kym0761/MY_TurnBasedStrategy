using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using TMPro;

public class GridDebugObject : MonoBehaviour
{
    //Grid 정보를 알려주는 Debug용 오브젝트. X Z 정보를 확인 가능함.

    [SerializeField] 
    private TextMeshPro textMeshPro;
    private object gridObject;
    public virtual void SetGridObject(object gridObject)
    {
        this.gridObject = gridObject;
    }

    protected virtual void Update()
    {
        textMeshPro.text = gridObject.ToString();
    }

}
