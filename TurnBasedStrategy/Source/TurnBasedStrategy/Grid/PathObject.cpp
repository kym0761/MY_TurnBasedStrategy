// Fill out your copyright notice in the Description page of Project Settings.


#include "PathObject.h"

UPathObject::UPathObject() :G_Cost(0), H_Cost(0), F_Cost(0), bIsWalkable(true), ParentObject(nullptr)
{

}

void UPathObject::SetGrid(FGrid InGrid)
{
	Grid = InGrid;

}

int32 UPathObject::GetGCost() const
{
	return G_Cost;
}

int32 UPathObject::GetHCost() const
{
	return H_Cost;
}

int32 UPathObject::GetFCost() const
{
	return F_Cost;
}

int32 UPathObject::GetGridCost() const
{
	return GridCost;
}

void UPathObject::SetGCost(int32 Value)
{
	G_Cost = Value;
}

void UPathObject::SetHCost(int32 Value)
{
	H_Cost = Value;
}

void UPathObject::SetFCost(int32 Value)
{
	F_Cost = Value;
}

void UPathObject::SetGridCost(int32 Value)
{
	GridCost = Value;
}

void UPathObject::CalculateFCost()
{
	F_Cost = H_Cost + G_Cost;
}

void UPathObject::SetParentObject(UPathObject* InPathObject)
{
	ParentObject = InPathObject;
}

UPathObject* UPathObject::GetParentObject() const
{
	return ParentObject;
}

FGrid UPathObject::GetGrid() const
{
	return Grid;
}

bool UPathObject::GetIsWalkable() const
{
	return bIsWalkable;
}

void UPathObject::SetIsWalkable(bool InVal)
{
	bIsWalkable = InVal;
}

