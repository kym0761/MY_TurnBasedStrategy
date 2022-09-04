using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Unit : MonoBehaviour
{
    [SerializeField]
    private Animator _UnitAnimator;
    private Vector3 _TargetPosition;
    private GridPosition gridPosition;
    // Start is called before the first frame update

    private void Awake()
    {
        _TargetPosition = transform.position;
    }
    void Start()
    {
        gridPosition = LevelGrid.Instance.GetGridPosition(transform.position);

        LevelGrid.Instance.AddUnitAtGridPosition(gridPosition,this);
    }

    // Update is called once per frame
    void Update()
    {
        float stoppingDistance = 0.1f;
        if (Vector3.Distance(transform.position, _TargetPosition) > stoppingDistance)
        {
            Vector3 moveDirection = (_TargetPosition - transform.position).normalized;
            float moveSpeed = 4.0f;
            transform.position += moveDirection * moveSpeed * Time.deltaTime;

            float rotationSpeed = 10.0f;
            transform.forward = Vector3.Lerp(transform.forward, moveDirection, rotationSpeed * Time.deltaTime);


            _UnitAnimator.SetBool("IsWalking", true);
        }
        else
        { 
            _UnitAnimator.SetBool("IsWalking", false); 
        }

        //if (Input.GetMouseButtonDown(0))
        //{
        //    Vector3 pos = MouseWorld.GetPosition();
        //    Move(pos);
        //}
        GridPosition newGridPosition = LevelGrid.Instance.GetGridPosition(transform.position);
        if (newGridPosition != gridPosition)
        {
            LevelGrid.Instance.UnitMovedGridPosition(this, gridPosition, newGridPosition);
            gridPosition = newGridPosition;
        }
    }

    public void Move(Vector3 TargetPosition)
    {
        _TargetPosition = TargetPosition;
    }

}
