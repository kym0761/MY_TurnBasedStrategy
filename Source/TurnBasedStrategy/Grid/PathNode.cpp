// Fill out your copyright notice in the Description page of Project Settings.


#include "PathNode.h"

UPathNode::UPathNode()
{
	G_Cost = 0;
	H_Cost = 0;
	F_Cost = 0;

	bIsWalkable = true;
	ParentNode = nullptr;
}

void UPathNode::SetGrid(FGrid InGrid)
{
	Grid = InGrid;

}

int32 UPathNode::GetGCost() const
{
	return G_Cost;
}

int32 UPathNode::GetHCost() const
{
	return H_Cost;
}

int32 UPathNode::GetFCost() const
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

UPathNode* UPathNode::GetParentNode() const
{
	return ParentNode;
}

FGrid UPathNode::GetGrid() const
{
	return Grid;
}

bool UPathNode::GetIsWalkable() const
{
	return bIsWalkable;
}

void UPathNode::SetIsWalkable(bool InVal)
{
	bIsWalkable = InVal;
}

