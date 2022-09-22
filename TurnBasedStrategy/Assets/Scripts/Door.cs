using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Door : MonoBehaviour, IInteractable
{
    [SerializeField] private bool isOpen;

    private GridPosition gridPosition;
    private Animator animator;
    private Action onInteractionComplete;
    private float timer;
    private bool isActive;


    private void Awake()
    {
        animator = GetComponent<Animator>();
    }

    private void Start()
    {
        gridPosition = LevelGrid.Instance.GetGridPosition(transform.position);
        LevelGrid.Instance.SetInteractableAtGridPosition(gridPosition, this);


        if (isOpen)
        {
            OpenDoor();
        }
        else
        {
            CloseDoor();
        }

    }

    private void Update()
    {
        if (!isActive)
        {
            return;
        }

        timer -= Time.deltaTime;

        if (timer <= 0.0f)
        {
            isActive = false;
            onInteractionComplete();
        }
    }

    public void Interact(Action onInteractionComplete)
    {
        this.onInteractionComplete = onInteractionComplete;
        isActive = true;
        timer = 0.5f;

        if (isOpen)
        {
            CloseDoor();
            
        }
        else
        {
            OpenDoor();
        }

    }


    private void OpenDoor()
    {
        isOpen = true;
        animator.SetBool("IsOpen" , isOpen);
        PathFinding.Instance.SetWalkableGridPosition(gridPosition, true);
    }

    private void CloseDoor()
    {
        isOpen = false;
        animator.SetBool("IsOpen", isOpen);
        PathFinding.Instance.SetWalkableGridPosition(gridPosition, false);
    }
}