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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	int32 Height;

	FGrid();
	FGrid(int32 _X, int32 _Y);
	FGrid(int32 _X, int32 _Y, int _Height);
	FString ToString();
	bool operator == (const FGrid& Other);
	friend bool operator==(const FGrid& a, const FGrid& b); // contains() function needs it!
	bool operator != (const FGrid& Other);
	friend bool operator!=(const FGrid& a, const FGrid& b);
	FGrid operator +(FGrid b);
	FGrid operator -(FGrid b);
};



UENUM(BlueprintType)
enum class EGridVisualType : uint8
{
	Move UMETA(DisplayName = "Move"), //�̵�?
	OK UMETA(DisplayName = "OK"), //�Ʊ�����?
	NO UMETA(DisplayName = "NO"), // ������?
	NO_Soft UMETA(DisplayName = "No_Soft"), //���� ������ ĭ
	Warning UMETA(DisplayName = "Warning") // ���� ������ ����?

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
};

//USTRUCT(BlueprintType)
//struct FGridVisualTypeMaterial
//{
//	GENERATED_BODY()
//public:
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
//		EGridVisualType GridVisualType;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
//		UMaterialInstance* Material;
//};