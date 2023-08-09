// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Grid.h"
#include "PathNode.generated.h"
/**
 * 
 */

UCLASS()
class TURNBASEDSTRATEGY_API UPathNode : public UObject
{
	GENERATED_BODY()
public:

	UPathNode();

private:

	UPROPERTY()
		int32 G_Cost; // ��������� cost
	UPROPERTY()
		int32 H_Cost; // ������ �ʿ��� ������ ����Ǵ� cost
	UPROPERTY()
		int32 F_Cost; // ���� cost

	UPROPERTY()
		FGrid Grid;
	UPROPERTY()
		bool bIsWalkable;
	UPROPERTY()
		UPathNode* ParentNode;

public:

	void SetGrid(FGrid InGrid);
	int32 GetGCost() const;
	int32 GetHCost() const;
	int32 GetFCost() const;

	void SetGCost(int32 Value);
	void SetHCost(int32 Value);
	void SetFCost(int32 Value);

	void CalculateFCost();

	void SetParentNode(UPathNode* InPathNode);
	UPathNode* GetParentNode() const;

	FGrid GetGrid() const;
	bool GetIsWalkable() const;
	void SetIsWalkable(bool InVal);

    FORCEINLINE bool operator<(const UPathNode& Other) const
    {
        return F_Cost < Other.GetFCost();
    }

    static bool PathFindingPredicated(const UPathNode& A, const UPathNode& B)
    {
        return A.GetFCost() < B.GetFCost();
    }
};

template <class InObject>
class TPriorityQueue
{
private:
	TArray<InObject> Arr;

public:

	//��� ���� : TPriorityQueue<UPathNode*> pq(UPathNode::PathFindingPredicated);

	template <class PREDICATE_CLASS>
	TPriorityQueue(const PREDICATE_CLASS& Predicate) : Arr(TArray<InObject>())
	{
		Arr.Heapify(Predicate);
	}

	InObject Pop()
	{
		InObject outObj;
		Arr.HeapPop(outObj);
		return outObj;
	}

	void Push(InObject ObjVal)
	{
		Arr.HeapPush(ObjVal);
	}

};