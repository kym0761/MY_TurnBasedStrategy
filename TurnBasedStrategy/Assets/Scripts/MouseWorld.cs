using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MouseWorld : MonoBehaviour
{
    //���콺�� ���� �ӿ� �� �ϳ��� ������.
    private static MouseWorld _Instance;

    [SerializeField]
    private LayerMask MousePlaneLayerMask;

    // Start is called before the first frame update
    void Awake()
    {
        _Instance = this;
    }

    // Update is called once per frame
    void Update()
    {
        transform.position = MouseWorld.GetPosition();
    }

    public static Vector3 GetPosition()
    {

        Ray ray = Camera.main.ScreenPointToRay(Input.mousePosition);
        RaycastHit raycastHit;
        Physics.Raycast(ray, out raycastHit, float.MaxValue, _Instance.MousePlaneLayerMask);
        return raycastHit.point;
    }

}
