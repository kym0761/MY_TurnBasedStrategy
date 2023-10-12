// Fill out your copyright notice in the Description page of Project Settings.


#include "InstancedGridVisualComponent.h"
#include "Manager/SRPG_GameMode.h"

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

	ASRPG_GameMode* gameMode = ASRPG_GameMode::GetSRPG_GameMode(GetWorld());
	if (!IsValid(gameMode))
	{
		//gameMode Not Valid.
		return;
	}

	//Set�� ���������� ���� �����Ƿ� �̸� Array�� �޾� �� ������� Instance�� ������ �����ϵ��� �Ѵ�.
	TArray<FGrid> gridArray = GridSet.Array();

	for (const FGrid& grid : gridArray)
	{
		if (!gameMode->IsValidGrid(grid))
		{
			continue;
		}

		FVector worldPos = gameMode->GridToWorld(grid);
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

		bool up = GridSet.Contains(upGrid);
		bool down = GridSet.Contains(downGrid);
		bool left = GridSet.Contains(leftGrid);
		bool right = GridSet.Contains(rightGrid);

		bool upLeft = (up && left) && GridSet.Contains(upLeftGrid);
		bool upRight = (up && right) && GridSet.Contains(upRightGrid);
		bool downLeft = (down && left) && GridSet.Contains(downLeftGrid);
		bool downRight = (down && right) && GridSet.Contains(downRightGrid);

		float upValue = up ? 0.0f : 1.0f;
		float downValue = down ? 0.0f : 1.0f;
		float leftValue = left ? 0.0f : 1.0f;
		float rightValue = right ? 0.0f : 1.0f;

		float upLeftValue = upLeft ? 0.0f : 1.0f;
		float upRightValue = upRight ? 0.0f : 1.0f;
		float downLeftValue = downLeft ? 0.0f : 1.0f;
		float downRightValue = downRight ? 0.0f : 1.0f;

		SetCustomDataValue(i, 0, upValue);
		SetCustomDataValue(i, 1, downValue);
		SetCustomDataValue(i, 2, leftValue);
		SetCustomDataValue(i, 3, rightValue);

		SetCustomDataValue(i, 4, upLeftValue);
		SetCustomDataValue(i, 5, upRightValue);
		SetCustomDataValue(i, 6, downLeftValue);
		SetCustomDataValue(i, 7, downRightValue);

	}

	
	for (int32 i = 0; i < gridArray.Num(); i++)
	{
		FGrid currentGrid = gridArray[i];

		//�����¿�
		FGrid upGrid = currentGrid + FGrid(0, 1);
		FGrid downGrid = currentGrid + FGrid(0, -1);
		FGrid leftGrid = currentGrid + FGrid(-1, 0);
		FGrid rightGrid = currentGrid + FGrid(1, 0);

		bool up = GridSet.Contains(upGrid);
		bool down = GridSet.Contains(downGrid);
		bool left = GridSet.Contains(leftGrid);
		bool right = GridSet.Contains(rightGrid);

		float upValue = up ? 0.0f : 1.0f;
		float downValue = down ? 0.0f : 1.0f;
		float leftValue = left ? 0.0f : 1.0f;
		float rightValue = right ? 0.0f : 1.0f;
	
		SetCustomDataValue(i, 0, upValue);
		SetCustomDataValue(i, 1, downValue);
		SetCustomDataValue(i, 2, leftValue);
		SetCustomDataValue(i, 3, rightValue);
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
