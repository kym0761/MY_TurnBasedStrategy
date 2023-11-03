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

UCLASS(BlueprintType, Blueprintable)
class TURNBASEDSTRATEGY_API UGridObject : public UObject
{
	GENERATED_BODY()

public:

	UGridObject();

private:

	//이 GridObj를 보유한 GridSystem.
	UPROPERTY()
	UGridSystem* GridSystem;

	//자신의 Grid정보
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Grid", Meta = (AllowPrivateAccess = true))
	FGrid Grid;

	//이 그리드를 접근하기 위해 필요한 Cost.
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Grid", Meta = (AllowPrivateAccess = true))
	int32 GridCost;

	//이 Grid 위에 올라온 유닛
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
	int32 GetGridCost() const;
	void SetGridCost(int32 Val);
};
