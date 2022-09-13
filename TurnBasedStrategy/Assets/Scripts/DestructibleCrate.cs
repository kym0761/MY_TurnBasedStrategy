using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class DestructibleCrate : MonoBehaviour
{
    [SerializeField] private Transform crate_DestroyedPrefab;

    public static event EventHandler onAnyDestroyed;    //PathFindingUpdater¿¡¼­ BindµÊ.

    private GridPosition gridPosition;

    // Start is called before the first frame update
    void Start()
    {
        gridPosition = LevelGrid.Instance.GetGridPosition(transform.position);
    }

    public GridPosition GetGridPosition()
    {
        return gridPosition;
    }

    public void Damage()
    {
        Transform crate_des_transform = Instantiate(crate_DestroyedPrefab, transform.position, transform.rotation);
        ApplyExplosionToChildren(crate_des_transform, 150.0f, transform.position, 10.0f);

        Destroy(gameObject);

        //Grenade
        onAnyDestroyed?.Invoke(this, EventArgs.Empty);
    }

    private void ApplyExplosionToChildren(Transform root, float explosionForce, Vector3 explosionPosition, float explosionRange)
    {
        foreach (Transform child in root)
        {
            if (child.TryGetComponent<Rigidbody>(out Rigidbody childRigidbody))
            {
                childRigidbody.AddExplosionForce(explosionForce, explosionPosition, explosionRange);
            }

            ApplyExplosionToChildren(child, explosionForce, explosionPosition, explosionRange);
        }
    }

}

