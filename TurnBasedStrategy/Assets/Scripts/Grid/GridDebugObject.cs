using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using TMPro;

public class GridDebugObject : MonoBehaviour
{
    //Grid ������ �˷��ִ� Debug�� ������Ʈ. X Z ������ Ȯ�� ������.

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
