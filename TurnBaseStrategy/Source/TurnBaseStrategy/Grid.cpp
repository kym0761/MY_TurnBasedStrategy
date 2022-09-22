// Fill out your copyright notice in the Description page of Project Settings.


#include "Grid.h"

FGrid::FGrid()
{
	X = 0;
	Y = 0;
	Height = 0;
}

FGrid::FGrid(int32 _X, int32 _Y)
{
	X = _X;
	Y = _Y;
	Height = 0;
}

FGrid::FGrid(int32 _X, int32 _Y, int _Height)
{
	X = _X;
	Y = _Y;
	Height = _Height;
}

FString FGrid::ToString()
{
	return FString("X : ") + FString::FromInt(X) + FString(" / Y : ") + FString::FromInt(Y) + FString(" / Height : ") + FString::FromInt(Height);
}

bool FGrid::operator == (const FGrid Other)
{
	return X == Other.X && Y == Other.Y && Height == Other.Height;
}

bool FGrid::operator != (const FGrid Other)
{
	return !(*this == Other);
}

FGrid FGrid::operator +(FGrid b)
{
	FGrid temp;
	temp.X = X + b.X;
	temp.Y = Y + b.Y;
	temp.Height = Height + b.Height;
	return temp;
}

FGrid FGrid::operator -(FGrid b)
{
	FGrid temp;
	temp.X = X - b.X;
	temp.Y = Y - b.Y;
	temp.Height = Height - b.Height;
	return temp;
}