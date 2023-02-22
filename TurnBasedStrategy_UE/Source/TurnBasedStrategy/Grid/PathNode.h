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

	int32 G_Cost; // 현재까지의 cost
	int32 H_Cost; // 앞으로 필요할 것으로 예상되는 cost
	int32 F_Cost; // 총합 cost

	FGrid Grid;
	bool bIsWalkable;
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

	bool operator<(const UPathNode* Other) const
	{
		return GetGCost() < Other->GetGCost();
	}

};


template <typename InElementType>
struct TPriorityQueueNode {
    InElementType Element;
    float Priority;

    TPriorityQueueNode()
    {
    }

    TPriorityQueueNode(InElementType InElement, float InPriority)
    {
        Element = InElement;
        Priority = InPriority;
    }

    bool operator<(const TPriorityQueueNode<InElementType> Other) const
    {
        return Priority < Other.Priority;
    }
};

template <typename InElementType>
class TPriorityQueue {
public:
    TPriorityQueue()
    {
        Array.Heapify();
    }

public:
    // Always check if IsEmpty() before Pop-ing!
    InElementType Pop()
    {
        TPriorityQueueNode<InElementType> Node;
        Array.HeapPop(Node);
        return Node.Element;
    }

    TPriorityQueueNode<InElementType> PopNode()
    {
        TPriorityQueueNode<InElementType> Node;
        Array.HeapPop(Node);
        return Node;
    }

    void Push(InElementType Element, float Priority)
    {
        Array.HeapPush(TPriorityQueueNode<InElementType>(Element, Priority));
    }

    bool IsEmpty() const
    {
        return Array.Num() == 0;
    }

    int32 Num() const
    {
        return Array.Num();
    }


private:
    TArray<TPriorityQueueNode<InElementType>> Array;
};
