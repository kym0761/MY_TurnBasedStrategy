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

	UPROPERTY()
	int32 X_Length;
	UPROPERTY()
	int32 Y_Length;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PathFinding", Meta = (AllowPrivateAccess = true))
		TMap<FGrid,UPathNode*> PathNodeMap;
public:

	UPathFindingSystem();


	//Call Order in GridManager : NewObject<>(); -> SetPathFindingSystem();
	void SetPathFindingSystem(int _X_Length, int _Y_Length,
		TFunctionRef<UPathNode* (UPathFindingSystem*, FGrid)> CreateObjectFunction);

	TMap<FGrid,UPathNode*> GetPathNodeMap() const;
	UPathNode* GetValidPathNode(const FGrid& Grid) const;

};
