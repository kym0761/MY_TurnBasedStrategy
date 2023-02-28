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
class TURNBASEDSTRATEGY_API UPathFindingSystem : public UObject
{
	GENERATED_BODY()

private:

	int32 X_Length;
	int32 Y_Length;

	UPROPERTY()
		TArray<UPathNode*> PathNodeArray;
public:

	UPathFindingSystem();


	//Call Order in GridManager : NewObject<>(); -> SetPathFindingSystem();
	void SetPathFindingSystem(int _X_Length, int _Y_Length,
		TFunctionRef<UPathNode* (UPathFindingSystem*, FGrid)> CreateObjectFunction);

	TArray<UPathNode*> GetPathNodeArray() const;

	UPathNode* GetValidPathNode(const FGrid& Grid) const;

};
