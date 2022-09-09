using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Cinemachine;

public class CameraController : MonoBehaviour
{
    //cinemachine의 virtual Camera를 사용하여 간단하게 카메라 구현.
    //Q E : 회전
    //W A S D : 이동
    //마우스 휠 : 줌

    [SerializeField]
    private CinemachineVirtualCamera cinemachineVirtualCamera;

    const float MIN_FOLLOW_Y_OFFSET = 2.0f;
    const float MAX_FOLLOW_Y_OFFSET = 12.0f;

    private Vector3 targetFollowOffset;
    private CinemachineTransposer transposer;


    void Start()
    {
        transposer = cinemachineVirtualCamera.GetCinemachineComponent<CinemachineTransposer>();
        targetFollowOffset = transposer.m_FollowOffset;
    }

    // Update is called once per frame
    void Update()
    {
        CameraMove();

        CameraRotate();

        CameraZoom();

    }

    private void CameraZoom()
    {
        float zoomAmount = 50.0f * Time.deltaTime;
        if (Input.mouseScrollDelta.y > 0)
        {
            targetFollowOffset.y -= zoomAmount;
        }
        if (Input.mouseScrollDelta.y < 0)
        {
            targetFollowOffset.y += zoomAmount;
        }

        targetFollowOffset.y = Mathf.Clamp(targetFollowOffset.y, MIN_FOLLOW_Y_OFFSET, MAX_FOLLOW_Y_OFFSET);

        float zoomSpeed = 5.0f;
        transposer.m_FollowOffset =
            Vector3.Lerp(transposer.m_FollowOffset, targetFollowOffset, Time.deltaTime * zoomSpeed);
    }

    private void CameraRotate()
    {
        Vector3 rotationVector = new Vector3(0, 0, 0);

        if (Input.GetKey(KeyCode.Q))
        {
            rotationVector.y = 1.0f;
        }
        if (Input.GetKey(KeyCode.E))
        {
            rotationVector.y = -1.0f;
        }

        float rotationSpeed = 100.0f;
        transform.eulerAngles += rotationVector * Time.deltaTime * rotationSpeed;
    }

    private void CameraMove()
    {
        Vector3 inputMoveDir = new Vector3(0, 0, 0);
        if (Input.GetKey(KeyCode.W))
        {
            inputMoveDir.z = 1.0f;
        }
        if (Input.GetKey(KeyCode.S))
        {
            inputMoveDir.z = -1.0f;
        }
        if (Input.GetKey(KeyCode.A))
        {
            inputMoveDir.x = -1.0f;
        }
        if (Input.GetKey(KeyCode.D))
        {
            inputMoveDir.x = 1.0f;
        }

        float moveSpeed = 10.0f;
        Vector3 moveVector = transform.forward * inputMoveDir.z + transform.right * inputMoveDir.x;
        moveVector.Normalize();
        transform.position += moveVector * Time.deltaTime * moveSpeed;
    }
}
