// Fill out your copyright notice in the Description page of Project Settings.


#include "InstancedGridVisualComponent.h"
#include "Manager/GridManager.h"

UInstancedGridVisualComponent::UInstancedGridVisualComponent()
{
	SetCollisionProfileName(TEXT("NoCollision"));

	SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));

	//��밡���� Material�� Custom Data Value�� �ִ� ����
	NumCustomDataFloats = 8;
}

void UInstancedGridVisualComponent::DrawGridVisualswithGridSet(const TSet<FGrid>& GridSet, const float Height)
{
	FTransformArrayA2 VisualTransformArray;

	AGridManager* gridManager = Cast<AGridManager>(GetOwner());
	if (!IsValid(gridManager))
	{
		//gridManager Not Valid.
		return;
	}

	//Set�� ���������� ���� �����Ƿ� �̸� Array�� �޾� �� ������� Instance�� ������ �����ϵ��� �Ѵ�.
	TArray<FGrid> gridArray = GridSet.Array();

	for (const FGrid& grid : gridArray)
	{
		if (!gridManager->IsValidGrid(grid))
		{
			continue;
		}

		FVector worldPos = gridManager->GridToWorld(grid);
		worldPos.Z += Height;

		FTransform visualTransform;
		visualTransform.SetLocation(worldPos);
		visualTransform.SetRotation(FQuat::Identity);
		visualTransform.SetScale3D(GetComponentScale());
		
		VisualTransformArray.Add(visualTransform);
	}

	//draw all
	AddInstances(VisualTransformArray, false, true);

	//Grid�� �ܰ� �׵θ��� ��Ÿ���� ���� ����ϴ� ����.
	//Material�� InstancedStaticMesh ������ ���¿��� Custom Data Value ����� index�� ���缭 ���� �־��ش�.
	//�� ����� ����Ϸ���
	//InstancedMesh�� NumCustomDataFloats�� ����� Custom Data Value ������ ���� �����ڿ� �������־�� ��.
	for (int32 i = 0; i < gridArray.Num(); i++)
	{
		FGrid currentGrid = gridArray[i];

		//�����¿�
		FGrid upGrid = currentGrid + FGrid(0,1);
		FGrid downGrid = currentGrid + FGrid(0, -1);
		FGrid leftGrid = currentGrid + FGrid(-1, 0);
		FGrid rightGrid = currentGrid + FGrid(1, 0);

		//�밢����
		FGrid upLeftGrid = currentGrid + FGrid(-1, 1);
		FGrid upRightGrid = currentGrid + FGrid(1, 1);
		FGrid downLeftGrid = currentGrid + FGrid(-1, -1);
		FGrid downRightGrid = currentGrid + FGrid(1, -1);

		float upValue = GridSet.Contains(upGrid) ? 0.0f : 1.0f;
		float downValue = GridSet.Contains(downGrid) ? 0.0f : 1.0f;
		float leftValue = GridSet.Contains(leftGrid) ? 0.0f : 1.0f;
		float rightValue = GridSet.Contains(rightGrid) ? 0.0f : 1.0f;

		float upLeftValue = GridSet.Contains(upLeftGrid) ? 0.0f : 1.0f;
		float upRightValue = GridSet.Contains(upRightGrid) ? 0.0f : 1.0f;
		float downLeftValue = GridSet.Contains(downLeftGrid) ? 0.0f : 1.0f;
		float downRightValue = GridSet.Contains(downRightGrid) ? 0.0f : 1.0f;

		SetCustomDataValue(i, 0, upValue);
		SetCustomDataValue(i, 1, downValue);
		SetCustomDataValue(i, 2, leftValue);
		SetCustomDataValue(i, 3, rightValue);

		SetCustomDataValue(i, 4, upLeftValue);
		SetCustomDataValue(i, 5, upRightValue);
		SetCustomDataValue(i, 6, downLeftValue);
		SetCustomDataValue(i, 7, downRightValue);

	}

	
}

void UInstancedGridVisualComponent::RemoveGridVisuals()
{
	if (GetInstanceCount() == 0)
	{
		return;
	}

	ClearInstances();
}
