// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Grid.h"
#include "InstancedGridVisualComponent.generated.h"

/**
 * �������� ���̴� Grid �ð� Component
 */
UCLASS()
class TURNBASEDSTRATEGY_API UInstancedGridVisualComponent : public UInstancedStaticMeshComponent
{
	GENERATED_BODY()
public:

	UInstancedGridVisualComponent();

public:

	void DrawGridVisualswithGridSet(const TSet<FGrid>& GridSet, const float Height = 0.01f);
	void RemoveGridVisuals();
};
