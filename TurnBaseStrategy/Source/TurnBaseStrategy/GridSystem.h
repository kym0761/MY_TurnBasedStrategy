// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Grid.h"
#include "GridSystem.generated.h"

/*
 * https://docs.unrealengine.com/5.0/en-US/API/Runtime/Core/GenericPlatform/TFunction/
 * https://stackoverflow.com/questions/1639797/template-issue-causes-linker-error-c
 * 
 * Pure C++ Class는 UObject의 GC을 막을 수 없음.
 */

class UGridObject;


UCLASS()
class TURNBASESTRATEGY_API UGridSystem : public UObject
{
	GENERATED_BODY()

private:
	int32 X_Length;
	int32 Y_Length;
	float CellSize;

	UPROPERTY()
		TArray<UGridObject*> GridObjectArray;

public:
	UGridSystem();

public:

	void SetGridSystem(int _X_Length, int _Y_Length, float _CellSize,
		TFunctionRef<UGridObject* (UGridSystem*, FGrid)> CreateObjectFunction);

	TArray<UGridObject*> GetGridObjectArray() const;

	FGrid WorldToGrid(const FVector& WorldPosition) const;
	FVector GridToWorld(const FGrid& Grid) const;

	UGridObject* GetValidGridObject(const FGrid& Grid) const;

};