// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Grid.generated.h"

/**
 * Grid ����
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

	FGrid();
	FGrid(int32 _X, int32 _Y);
	FString ToString() const;
	bool operator==(const FGrid& Other) const;
	bool operator!= (const FGrid& Other) const;
	FGrid operator+(const FGrid& b) const;
	FGrid operator-(const FGrid& b) const;
	FGrid& operator=(const FGrid& b);
	FGrid& operator+=(const FGrid& b);
	FGrid& operator-=(const FGrid& b);
	int32 Size() const;
};

//To Use in TSet Or TMap. You Need To Make operator==, GetTypeHash(). 
uint32 GetTypeHash(const FGrid& Grid)
{
	return FCrc::MemCrc32(&Grid, sizeof(Grid));
}


UENUM(BlueprintType)
enum class EGridVisualType : uint8
{
	Move UMETA(DisplayName = "Move"), //�̵�?
	OK UMETA(DisplayName = "OK"), //�Ʊ�����?
	NO UMETA(DisplayName = "NO"), // ������?
	NO_Soft UMETA(DisplayName = "No_Soft"), //���� ������ ĭ
	Warning UMETA(DisplayName = "Warning"), // ���� ������ ����?
	DANGER UMETA(DisplayName = "DANGER") // ���� ����?
};

USTRUCT(BlueprintType)
struct FGridVisualData
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
		FGrid Grid;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
		EGridVisualType GridVisualType;

	static TArray<FGrid> ParseToGridArray(TArray<FGridVisualData> GridVisualArray);
	bool operator==(const FGridVisualData& Other) const;
};

//To Use in TSet Or TMap. You Need To Make operator==, GetTypeHash(). 
uint32 GetTypeHash(const FGridVisualData& GridVisualData)
{
	return FCrc::MemCrc32(&GridVisualData, sizeof(FGridVisualData));
}