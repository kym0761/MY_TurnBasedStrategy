// Fill out your copyright notice in the Description page of Project Settings.


#include "PathNode.h"

UPathNode::UPathNode()
{
	G_Cost = 1;
	H_Cost = 2;
	F_Cost = 3;

	bIsWalkable = true;
	ParentNode = nullptr;
}

void UPathNode::SetGrid(FGrid InGrid)
{
	Grid = InGrid;

}

int32 UPathNode::GetGCost()
{
	return G_Cost;
}

int32 UPathNode::GetHCost()
{
	return H_Cost;
}

int32 UPathNode::GetFCost()
{
	return F_Cost;
}

void UPathNode::SetGCost(int32 Value)
{
	G_Cost = Value;
}

void UPathNode::SetHCost(int32 Value)
{
	H_Cost = Value;
}

void UPathNode::SetFCost(int32 Value)
{
	F_Cost = Value;
}

void UPathNode::CalculateFCost()
{
	F_Cost = H_Cost + G_Cost;
}

void UPathNode::SetParentNode(UPathNode* InPathNode)
{
	ParentNode = InPathNode;
}

UPathNode* UPathNode::GetParentNode()
{
	return ParentNode;
}

FGrid UPathNode::GetGrid()
{
	return Grid;
}

bool UPathNode::GetIsWalkable()
{
	return bIsWalkable;
}

void UPathNode::SetIsWalkable(bool InVal)
{
	bIsWalkable = InVal;
}
