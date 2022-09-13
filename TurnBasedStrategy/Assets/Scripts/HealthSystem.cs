using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class HealthSystem : MonoBehaviour
{
    public event EventHandler onDead;
    public event EventHandler OnDamaged;
    [SerializeField] private int health = 100;
    private int healthMax;


    private void Awake()
    {
        healthMax = health;
    }


    public void Damage(int damageAmount)
    {
        health -= damageAmount;

        if (health < 0)
        {
            health = 0;
        }

        OnDamaged?.Invoke(this, EventArgs.Empty);

        if (health == 0)
        {
            Die();
        }

    }

    private void Die()
    {
        //RagdollSpawner에서 Ragdoll Spawn 이벤트가 bind 되어 있음.
        onDead?.Invoke(this, EventArgs.Empty);
    }

    public float GetHealthNormalized()
    {
        return (float)health / healthMax;
    }

}
