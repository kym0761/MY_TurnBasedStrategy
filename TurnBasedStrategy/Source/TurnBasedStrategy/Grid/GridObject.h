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

	//�� GridObj�� ������ GridSystem.
	UPROPERTY()
	UGridSystem* GridSystem;

	//�ڽ��� Grid����
	UPROPERTY()
	FGrid Grid;

	//�� �׸��带 �����ϱ� ���� �ʿ��� Cost.
	UPROPERTY()
	int32 GridCost;

	//�� Grid ���� �ö�� ����
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
