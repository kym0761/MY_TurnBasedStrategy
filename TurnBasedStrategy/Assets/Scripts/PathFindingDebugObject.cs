using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;

public class PathFindingDebugObject : GridDebugObject
{
    [SerializeField] private TextMeshPro gCostText;
    [SerializeField] private TextMeshPro hCostText;
    [SerializeField] private TextMeshPro fCostText;
    [SerializeField] private SpriteRenderer isWalkableSprite;

    private PathNode pathNode;

    public override void SetGridObject(object gridObject)
    {
        base.SetGridObject(gridObject);

        pathNode = gridObject as PathNode;
    }

    protected override void Update()
    {
        base.Update();

        gCostText.text = pathNode.GetGCost().ToString();
        hCostText.text = pathNode.GetHCost().ToString();
        fCostText.text = pathNode.GetFCost().ToString();


        isWalkableSprite.color = pathNode.IsWalkable() ? Color.green : Color.red;
    }

}
