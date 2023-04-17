// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Grid.h"
#include "GridObject.generated.h"

/**
 * 
 */

class AUnit;
class UGridSystem;

UCLASS()
class TURNBASEDSTRATEGY_API UGridObject : public UObject
{
	GENERATED_BODY()

public:

	UGridObject();

private:

	UPROPERTY()
		UGridSystem* GridSystem;

	UPROPERTY()
		FGrid Grid;

	UPROPERTY()
		TArray<AUnit*> UnitArray;

public:

	FString ToString() const;
	void AddUnit(AUnit* Unit);
	void RemoveUnit(AUnit* Unit);
	bool HasAnyUnit() const;
	AUnit* GetUnit() const;
	TArray<AUnit*> GetUnitArray() const;
	void SetGrid(FGrid InGrid);
	FGrid GetGrid() const;

	UGridSystem* GetGridSystem() const;
	void SetGridSystem(UGridSystem* InGridSystem);
};
