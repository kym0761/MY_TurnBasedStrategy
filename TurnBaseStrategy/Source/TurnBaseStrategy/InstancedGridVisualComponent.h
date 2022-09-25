// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Grid.h"
#include "InstancedGridVisualComponent.generated.h"

/**
 * 
 */
UCLASS()
class TURNBASESTRATEGY_API UInstancedGridVisualComponent : public UInstancedStaticMeshComponent
{
	GENERATED_BODY()
public:

	UInstancedGridVisualComponent();

public:

	void DrawGridVisualswithGridArray(TArray<FGrid> GridArray);
	void RemoveGridVisuals();
};
