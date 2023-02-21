// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Grid.h"
#include "GridObject.generated.h"

/**
 * 
 */

class AUnitCharacter;
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
		TArray<AUnitCharacter*> UnitArray;

public:

	FString ToString() const;
	void AddUnit(AUnitCharacter* Unit);
	void RemoveUnit(AUnitCharacter* Unit);
	bool HasAnyUnit() const;
	AUnitCharacter* GetUnit() const;
	TArray<AUnitCharacter*> GetUnitArray() const;
	void SetGrid(FGrid InGrid);
	FGrid GetGrid() const;

	UGridSystem* GetGridSystem() const;
	void SetGridSystem(UGridSystem* InGridSystem);
};
