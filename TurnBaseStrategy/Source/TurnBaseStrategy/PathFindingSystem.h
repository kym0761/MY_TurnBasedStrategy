// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Grid.h"
#include "PathFindingSystem.generated.h"

class UPathNode;

/**
 * 
 */
UCLASS()
class TURNBASESTRATEGY_API UPathFindingSystem : public UObject
{
	GENERATED_BODY()

private:

	int32 X_Length;
	int32 Y_Length;
	float CellSize;

	UPROPERTY()
		TArray<UPathNode*> PathNodeArray;
public:

	UPathFindingSystem();

	//FGridSystem();
	//~FGridSystem();
	//FGridSystem(int _X_Length, int _Y_Length, float _CellSize);

	void SetPathFindingSystem(int _X_Length, int _Y_Length, float _CellSize,
		TFunctionRef<UPathNode* (UPathFindingSystem*, FGrid)> CreateObjectFunction);

	TArray<UPathNode*> GetPathNodeArray() const;

	FGrid WorldToGrid(FVector WorldPosition) const;
	FVector GridToWorld(FGrid Grid) const;

	UPathNode* GetValidPathNode(FGrid Grid) const;

};
