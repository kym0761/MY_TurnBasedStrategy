// Fill out your copyright notice in the Description page of Project Settings.


#include "InstancedGridVisualComponent.h"
#include "Manager/GridManager.h"

UInstancedGridVisualComponent::UInstancedGridVisualComponent()
{
	SetCollisionProfileName(TEXT("NoCollision"));

	SetRelativeScale3D(FVector(0.95f, 0.95f, 0.95f));
}

void UInstancedGridVisualComponent::DrawGridVisualswithGridArray(const TArray<FGrid>& GridArray)
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

		FTransform visualTransform;
		visualTransform.SetLocation(worldPos);
		visualTransform.SetRotation(FQuat::Identity);
		visualTransform.SetScale3D(GetComponentScale());

		VisualTransformArray.Add(visualTransform);
	}

	//UE_LOG(LogTemp, Warning, TEXT("%s draw calling in DrawGridVisualswithGridArray"),*GetName());
	AddInstances(VisualTransformArray, false, true);

}

void UInstancedGridVisualComponent::DrawGridVisualsWithGridVisualData(const FGridVisualData& GridVisualData)
{
	AGridManager* gridManager = Cast<AGridManager>(GetOwner());
	if (!IsValid(gridManager))
	{
		//gridManager Not Valid.
		return;
	}

	FVector worldPos = gridManager->GridToWorld(GridVisualData.Grid);

	FTransform visualTransform;
	visualTransform.SetLocation(worldPos);
	visualTransform.SetRotation(FQuat::Identity);
	visualTransform.SetScale3D(GetComponentScale());

	//UE_LOG(LogTemp, Warning, TEXT("%s draw calling in DrawGridVisualsWithGridVisualData"), *GetName());

	AddInstance(visualTransform,true);
}

void UInstancedGridVisualComponent::RemoveGridVisuals()
{
	if (GetInstanceCount() == 0)
	{
		return;
	}

	ClearInstances();
}
