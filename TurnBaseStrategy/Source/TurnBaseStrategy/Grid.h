// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Grid.generated.h"
/**
 * 
 */
USTRUCT(BlueprintType, Blueprintable)
struct FGrid
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	int32 X;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	int32 Y;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
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