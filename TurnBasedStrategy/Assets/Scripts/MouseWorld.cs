using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MouseWorld : MonoBehaviour
{
    //���콺�� ���� �ӿ� �� �ϳ��� ������.
    private static MouseWorld Instance;

    [SerializeField]
    private LayerMask mousePlaneLayerMask; //MousePlane �����ʿ�

    // Start is called before the first frame update
    void Awake()
    {
        Instance = this;
    }

    // Update is called once per frame
    void Update()
    {
        transform.position = MouseWorld.GetPosition();
    }

    public static Vector3 GetPosition()
    {
        Ray ray = Camera.main.ScreenPointToRay(InputManager.Instance.GetMouseScreenPosition());
        RaycastHit raycastHit;
        Physics.Raycast(ray, out raycastHit, float.MaxValue, Instance.mousePlaneLayerMask);
        return raycastHit.point;
    }

}
