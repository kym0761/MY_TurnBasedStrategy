// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Grid.h"
#include "PathNode.generated.h"
/**
 * 
 */


UCLASS()
class TURNBASESTRATEGY_API UPathNode : public UObject
{
	GENERATED_BODY()
public:

	UPathNode();
private:
	int32 G_Cost;
	int32 H_Cost;
	int32 F_Cost;

	FGrid Grid;
	bool bIsWalkable;
	UPathNode* ParentNode;

public:
	void SetGrid(FGrid InGrid);
	int32 GetGCost();
	int32 GetHCost();
	int32 GetFCost();

	void SetGCost(int32 Value);
	void SetHCost(int32 Value);
	void SetFCost(int32 Value);

	void CalculateFCost();

	void SetParentNode(UPathNode* InPathNode);
	UPathNode* GetParentNode();

	FGrid GetGrid();
	bool GetIsWalkable();
	void SetIsWalkable(bool InVal);


};