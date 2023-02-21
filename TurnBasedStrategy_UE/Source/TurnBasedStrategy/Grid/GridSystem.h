// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Grid.h"
#include "GridSystem.generated.h"

/*
 * https://docs.unrealengine.com/5.0/en-US/API/Runtime/Core/GenericPlatform/TFunction/
 * 
 */

class UGridObject;


UCLASS()
class TURNBASEDSTRATEGY_API UGridSystem : public UObject
{
	GENERATED_BODY()

private:
	int32 X_Length;
	int32 Y_Length;

	UPROPERTY()
		TArray<UGridObject*> GridObjectArray;

public:
	UGridSystem();

public:

	void SetGridSystem(int _X_Length, int _Y_Length,
		TFunctionRef<UGridObject* (UGridSystem*, FGrid)> CreateObjectFunction);

	TArray<UGridObject*> GetGridObjectArray() const;
	UGridObject* GetValidGridObject(const FGrid& Grid) const;

};
