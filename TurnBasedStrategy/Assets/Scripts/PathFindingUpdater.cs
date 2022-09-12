using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PathFindingUpdater : MonoBehaviour
{
    // Start is called before the first frame update
    void Start()
    {
        DestructibleCrate.onAnyDestroyed += DestructibleCrate_OnAnyDestroyed;
    }

    private void DestructibleCrate_OnAnyDestroyed(object sender, EventArgs e)
    {
        DestructibleCrate crate = sender as DestructibleCrate;
        PathFinding.Instance.SetWalkableGridPosition(crate.GetGridPosition(), true);


    }

    // Update is called once per frame
    void Update()
    {
        
    }
}
