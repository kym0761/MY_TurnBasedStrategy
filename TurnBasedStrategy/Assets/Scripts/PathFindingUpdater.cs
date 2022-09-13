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
        //부셔진 Crate 위치를 걷기 가능한 위치로 변환.

        DestructibleCrate crate = sender as DestructibleCrate;
        PathFinding.Instance.SetWalkableGridPosition(crate.GetGridPosition(), true);
    }

    //// Update is called once per frame
    //void Update()
    //{
        
    //}
}
