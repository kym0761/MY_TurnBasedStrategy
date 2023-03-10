// Fill out your copyright notice in the Description page of Project Settings.


#include "InstancedGridVisualComponent.h"
#include "Manager/GridManager.h"

UInstancedGridVisualComponent::UInstancedGridVisualComponent()
{
	SetCollisionProfileName(TEXT("NoCollision"));

	SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));

	NumCustomDataFloats = 4;
}

void UInstancedGridVisualComponent::DrawGridVisualswithGridArray(const TArray<FGrid>& GridArray, const float Height)
{
	FTransformArrayA2 VisualTransformArray;

	AGridManager* gridManager = Cast<AGridManager>(GetOwner());
	if (!IsValid(gridManager))
	{
		//gridManager Not Valid.
		return;
	}

	for (const FGrid& grid : GridArray)
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

	//UE_LOG(LogTemp, Warning, TEXT("%s draw calling in DrawGridVisualswithGridArray"),*GetName());
	AddInstances(VisualTransformArray, false, true);

	for (int32 i = 0; i < GridArray.Num(); i++)
	{

		FGrid currentGrid = GridArray[i];
		FGrid upGrid = currentGrid + FGrid(0,1);
		FGrid downGrid = currentGrid + FGrid(0, -1);
		FGrid leftGrid = currentGrid + FGrid(-1, 0);
		FGrid rightGrid = currentGrid + FGrid(1, 0);

		float upValue = GridArray.Contains(upGrid) ? 0.0f : 1.0f;
		float downValue = GridArray.Contains(downGrid) ? 0.0f : 1.0f;
		float leftValue = GridArray.Contains(leftGrid) ? 0.0f : 1.0f;
		float rightValue = GridArray.Contains(rightGrid) ? 0.0f : 1.0f;

		if (upValue > 0.0f)
		{
			SetCustomDataValue(i, 0, upValue);
		}
		
		if (downValue > 0.0f)
		{
			SetCustomDataValue(i, 1, downValue);
		}
		
		if (leftValue > 0.0f)
		{
			SetCustomDataValue(i, 2, leftValue);
		}
		
		if (rightValue > 0.0f)
		{
			SetCustomDataValue(i, 3, rightValue);
		}
		
	}

	
}

//void UInstancedGridVisualComponent::DrawGridVisualsWithGridVisualData(const FGridVisualData& GridVisualData, const float Height)
//{
//	AGridManager* gridManager = Cast<AGridManager>(GetOwner());
//	if (!IsValid(gridManager))
//	{
//		//gridManager Not Valid.
//		return;
//	}
//
//	FVector worldPos = gridManager->GridToWorld(GridVisualData.Grid);
//	worldPos.Z += Height;
//
//	FTransform visualTransform;
//	visualTransform.SetLocation(worldPos);
//	visualTransform.SetRotation(FQuat::Identity);
//	visualTransform.SetScale3D(GetComponentScale());
//
//	//UE_LOG(LogTemp, Warning, TEXT("%s draw calling in DrawGridVisualsWithGridVisualData"), *GetName());
//
//	int32 i = AddInstance(visualTransform,true);
//
//
//	//FGrid currentGrid = GridVisualData.Grid;
//	//FGrid upGrid = currentGrid + FGrid(0, 1);
//	//FGrid downGrid = currentGrid + FGrid(0, -1);
//	//FGrid leftGrid = currentGrid + FGrid(-1, 0);
//	//FGrid rightGrid = currentGrid + FGrid(1, 0);
//
//	//float upValue = GridArray.Contains(upGrid) ? 0.0f : 1.0f;
//	//int32 downValue = GridArray.Contains(downGrid) ? 0.0f : 1.0f;
//	//int32 leftValue = GridArray.Contains(leftGrid) ? 0.0f : 1.0f;
//	//int32 rightValue = GridArray.Contains(rightGrid) ? 0.0f : 1.0f;
//
//	//SetCustomDataValue(i, 0, upValue);
//	//SetCustomDataValue(i, 1, downValue);
//	//SetCustomDataValue(i, 2, leftValue);
//	//SetCustomDataValue(i, 3, rightValue);
//	//
//
//
//}

void UInstancedGridVisualComponent::RemoveGridVisuals()
{
	if (GetInstanceCount() == 0)
	{
		return;
	}

	ClearInstances();
}
