using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ScreenShakeActions : MonoBehaviour
{



    // Start is called before the first frame update
    void Start()
    {

        ShootAction.onAnyShoot += ShootAction_OnAnyShoot;
        GrenadeProjectile.onAnyGranadeExploded += GrenadeProjectile_OnAnyGranadeExploded;

    }

    private void GrenadeProjectile_OnAnyGranadeExploded(object sender, EventArgs e)
    {
        ScreenShake.Instance.Shake(5.0f);
    }

    private void ShootAction_OnAnyShoot(object sender, ShootAction.OnShootEventArgs e)
    {
        ScreenShake.Instance.Shake();
    }
}
