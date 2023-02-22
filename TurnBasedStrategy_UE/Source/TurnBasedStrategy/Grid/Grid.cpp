// Fill out your copyright notice in the Description page of Project Settings.


#include "Grid.h"

FGrid::FGrid()
{
	X = 0;
	Y = 0;
}

FGrid::FGrid(int32 _X, int32 _Y)
{
	X = _X;
	Y = _Y;
}

FString FGrid::ToString() const
{
	return FString("X : ") + FString::FromInt(X) + FString(" / Y : ") + FString::FromInt(Y);
}

bool FGrid::operator==(const FGrid& Other) const
{
	return X == Other.X && Y == Other.Y;
}

bool FGrid::operator!=(const FGrid& Other) const
{
	return !(*this == Other);
}

FGrid FGrid::operator+(const FGrid& b) const
{
	FGrid temp;
	temp.X = X + b.X;
	temp.Y = Y + b.Y;
	return temp;
}

FGrid FGrid::operator -(const FGrid& b) const
{
	FGrid temp;
	temp.X = X - b.X;
	temp.Y = Y - b.Y;
	return temp;
}

FGrid& FGrid::operator=(const FGrid& b)
{
	X = b.X;
	Y = b.Y;

	return *this;
}

FGrid& FGrid::operator+=(const FGrid& b)
{
	(*this) = (*this) + b;
	return *this;
}

FGrid& FGrid::operator-=(const FGrid& b)
{
	(*this) = (*this) - b;
	return *this;
}

TArray<FGrid> FGridVisualData::ParseToGridArray(TArray<FGridVisualData> GridVisualArray)
{
	TArray<FGrid> gridArray;

	for (FGridVisualData visualData : GridVisualArray)
	{
		gridArray.Add(visualData.Grid);
	}

	return gridArray;
}
