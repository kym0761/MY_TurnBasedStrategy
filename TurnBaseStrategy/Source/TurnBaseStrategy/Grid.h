// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Grid.generated.h"
/**
 * 
 */
USTRUCT()
struct FGrid
{
	GENERATED_BODY()

public:

	int32 X;
	int32 Y;
	int32 Height;

	FGrid()
	{
		X = 0;
		Y = 0;
		Height = 0;
	}

	FGrid(int32 _X, int32 _Y)
	{
		X = _X;
		Y = _Y;
		Height = 0;
	}

	FGrid(int32 _X, int32 _Y, int _Height)
	{
		X = _X;
		Y = _Y;
		Height = _Height;
	}

	FString ToString()
	{
		return FString("X : ") + FString::FromInt(X) + FString(" / Y : ") + FString::FromInt(Y) + FString(" / Height : ") + FString::FromInt(Height);
	}

	bool operator == (const FGrid Other)
	{
		return X == Other.X && Y == Other.Y && Height == Other.Height;
	}

	bool operator != (const FGrid Other)
	{
		return !(*this == Other);
	}

	FGrid operator +(FGrid b)
	{
		FGrid temp;
		temp.X = X + b.X;
		temp.Y = Y + b.Y;
		temp.Height = Height + b.Height;
		return temp;
	}

	FGrid operator -(FGrid b)
	{
		FGrid temp;
		temp.X = X - b.X;
		temp.Y = Y - b.Y;
		temp.Height = Height - b.Height;
		return temp;
	}

};