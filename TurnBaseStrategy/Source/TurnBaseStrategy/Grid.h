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

	FGrid();
	FGrid(int32 _X, int32 _Y);
	FGrid(int32 _X, int32 _Y, int _Height);
	FString ToString();
	bool operator == (const FGrid Other);
	bool operator != (const FGrid Other);
	FGrid operator +(FGrid b);
	FGrid operator -(FGrid b);
};