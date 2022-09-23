// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Grid.h"
#include "GridSystem.h"
//#include "UObject/NoExportTypes.h"
#include "GridObject.generated.h"

/**
 * 
 */

class AUnitCharacter;

template<typename T>
class FGridSystem;

UCLASS()
class TURNBASESTRATEGY_API UGridObject : public UObject
{
	GENERATED_BODY()

public:

	UGridObject();

private:

	TWeakPtr<FGridSystem<UGridObject>> GridSystem;

	FGrid Grid;
	TArray<AUnitCharacter*> UnitArray;

public:

	FString ToString();
	void AddUnit(AUnitCharacter* Unit);
	void RemoveUnit(AUnitCharacter* Unit);
	bool HasAnyUnit();
	AUnitCharacter* GetUnit();
	void SetGrid(FGrid InGrid);
	FGrid GetGrid() const;

	TWeakPtr<FGridSystem<UGridObject>> GetGridSystem() const;
	void SetGridSystem(TSharedPtr<FGridSystem<UGridObject>> InGridSystem);
};
