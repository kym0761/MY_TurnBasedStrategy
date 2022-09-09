using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MoveAction : MonoBehaviour
{
    [SerializeField]
    private Animator unitAnimator;
    private Vector3 targetPosition;
    [SerializeField]
    private int maxMoveDistance = 4;
    private Unit unit;

    private void Awake()
    {
        //최초 위치 설정하여 움직이지 않게 막음.
        targetPosition = transform.position;
        unit = GetComponent<Unit>();
    }

    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        float stoppingDistance = 0.1f;

        //targetPosition으로 이동.
        if (Vector3.Distance(transform.position, targetPosition) > stoppingDistance)
        {
            Vector3 moveDirection = (targetPosition - transform.position).normalized;
            float moveSpeed = 4.0f;
            transform.position += moveDirection * moveSpeed * Time.deltaTime;

            float rotationSpeed = 10.0f;
            transform.forward = Vector3.Lerp(transform.forward, moveDirection, rotationSpeed * Time.deltaTime);

            //Animator 참고
            unitAnimator.SetBool("IsWalking", true);
        }
        else
        {
            unitAnimator.SetBool("IsWalking", false);
        }
    }

    public void Move(GridPosition gridPosition)
    {
        targetPosition = LevelGrid.Instance.GetWorldPosition(gridPosition);
    }

    public bool IsValidActionGridPosition(GridPosition gridPosition)
    {
        //ValidGrid인지 확인.

        return GetValidActionGridPositionList().Contains(gridPosition);
    }

    public List<GridPosition> GetValidActionGridPositionList()
    {
        //unit의 이동 범위 Grid에서 Valid한 Grid만 걸러냄

        List<GridPosition> validList = new List<GridPosition>();
        GridPosition unitGridPosition = unit.GetGridPosition();

        for (int x = -maxMoveDistance; x <= maxMoveDistance; x++)
        {
            for (int z = -maxMoveDistance; z <= maxMoveDistance; z++)
            {
                GridPosition offsetposition = new GridPosition(x, z);

                GridPosition testPosition = unitGridPosition + offsetposition;

                //없는 Grid
                if (!LevelGrid.Instance.IsValidGridPosition(testPosition))
                {
                    continue;
                }

                //자기 자신
                if (unitGridPosition == testPosition)
                {
                    continue;
                }

                //누군가 점유
                if (LevelGrid.Instance.HasAnyUnitOnGridPosition(testPosition))
                {
                    continue;
                }

                //통과했다면 문제 없으니 valid에 추가.
                validList.Add(testPosition);
            }
        }

        return validList;
    }
}
