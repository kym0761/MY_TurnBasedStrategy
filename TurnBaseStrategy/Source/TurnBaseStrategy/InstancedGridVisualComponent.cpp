// Fill out your copyright notice in the Description page of Project Settings.


#include "InstancedGridVisualComponent.h"
#include "GridManager.h"

UInstancedGridVisualComponent::UInstancedGridVisualComponent()
{

	SetRelativeScale3D(FVector(0.95f, 0.95f, 0.95f));
}

void UInstancedGridVisualComponent::DrawGridVisualswithGridArray(TArray<FGrid> GridArray)
{
	FTransformArrayA2 VisualTransformArray;

	AGridManager* gridManager = Cast<AGridManager>(GetOwner());
	if (!IsValid(gridManager))
	{
		//gridManager Not Valid.
		return;
	}

	for (FGrid grid : GridArray)
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

	AddInstances(VisualTransformArray, false, true);

}

void UInstancedGridVisualComponent::RemoveGridVisuals()
{
	ClearInstances();
}
