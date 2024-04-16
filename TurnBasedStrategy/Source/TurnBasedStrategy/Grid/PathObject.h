// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Grid.h"
#include "PathObject.generated.h"
/**
 * 
 */

UCLASS()
class TURNBASEDSTRATEGY_API UPathObject : public UObject
{
	GENERATED_BODY()
public:

	UPathObject();

private:

	// 현재까지의 cost
	UPROPERTY()
	int32 G_Cost;
	// 앞으로 필요할 것으로 예상되는 cost
	UPROPERTY()
	int32 H_Cost;
	// 총합 cost
	UPROPERTY()
	int32 F_Cost;
	//이 Pathnode가 담당하는 Grid로 진입하기 위한 Cost
	UPROPERTY()
	int32 GridCost;

	UPROPERTY()
	FGrid Grid;
	UPROPERTY()
	bool bIsWalkable;
	UPROPERTY()
	UPathObject* ParentObject;

public:

	void SetGrid(FGrid InGrid);
	int32 GetGCost() const;
	int32 GetHCost() const;
	int32 GetFCost() const;
	int32 GetGridCost() const;

	void SetGCost(int32 Value);
	void SetHCost(int32 Value);
	void SetFCost(int32 Value);
	void SetGridCost(int32 Value);

	void CalculateFCost();

	void SetParentObject(UPathObject* InPathObject);
	UPathObject* GetParentObject() const;

	FGrid GetGrid() const;
	bool GetIsWalkable() const;
	void SetIsWalkable(bool InVal);

    FORCEINLINE bool operator<(const UPathObject& Other) const
    {
        return F_Cost < Other.GetFCost();
    }

    static bool PathFindingPredicated(const UPathObject& A, const UPathObject& B)
    {
        return A.GetFCost() < B.GetFCost();
    }
};
