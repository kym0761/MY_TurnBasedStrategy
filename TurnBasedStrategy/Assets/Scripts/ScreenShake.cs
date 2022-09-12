using Cinemachine;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ScreenShake : MonoBehaviour
{
    public static ScreenShake Instance { get; private set; }

    private CinemachineImpulseSource impulseSource;

    private void Awake()
    {
        if (Instance != null)
        {
            Debug.LogError("There's more than one ScreenShake! " + transform + " - " + Instance);
            Destroy(gameObject);
            return;
        }
        Instance = this;

        impulseSource = GetComponent<CinemachineImpulseSource>();
    }


    public void Shake(float intensity = 1.0f)
    {
        impulseSource.GenerateImpulse(intensity);
    }
}