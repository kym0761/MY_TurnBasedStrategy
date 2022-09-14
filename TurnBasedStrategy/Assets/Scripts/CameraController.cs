using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Cinemachine;

public class CameraController : MonoBehaviour
{
    //cinemachine�� virtual Camera�� ����Ͽ� �����ϰ� ī�޶� ����.
    //Q E : ȸ��
    //W A S D : �̵�
    //���콺 �� : ��

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
        float zoomIncreaseAmount = 1f;
        targetFollowOffset.y += InputManager.Instance.GetCameraZoomAmount() * zoomIncreaseAmount;


        targetFollowOffset.y = Mathf.Clamp(targetFollowOffset.y, MIN_FOLLOW_Y_OFFSET, MAX_FOLLOW_Y_OFFSET);

        float zoomSpeed = 5.0f;
        transposer.m_FollowOffset =
            Vector3.Lerp(transposer.m_FollowOffset, targetFollowOffset, Time.deltaTime * zoomSpeed);
    }

    private void CameraRotate()
    {
        Vector3 rotationVector = new Vector3(0, 0, 0);

        rotationVector.y = InputManager.Instance.GetCameraRotateAmount();

        float rotationSpeed = 100.0f;
        transform.eulerAngles += rotationVector * Time.deltaTime * rotationSpeed;
    }

    private void CameraMove()
    {
        Vector2 inputMoveDir = InputManager.Instance.GetCameraMoveVector();

        float moveSpeed = 10.0f;
        Vector3 moveVector = transform.forward * inputMoveDir.y + transform.right * inputMoveDir.x;
        moveVector.Normalize();
        transform.position += moveVector * Time.deltaTime * moveSpeed;
    }
}
