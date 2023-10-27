// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Grid/Grid.h"
#include "GridManagerInterface.generated.h"

class AUnit;
class UGridObject;
class UPathObject;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UGridManagerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TURNBASEDSTRATEGY_API IGridManagerInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	/*GridSystem Functions*/
	virtual TArray<AUnit*> GetUnitArrayAtGrid(const FGrid& GridValue) const = 0;
	virtual TArray<AUnit*> GetAllUnitInGridSystem() const = 0;
	virtual AUnit* GetUnitAtGrid(const FGrid& GridValue) const = 0;
	virtual bool HasAnyUnitOnGrid(const FGrid& GridValue) const = 0;
	virtual bool IsWalkableGrid(const FGrid& GridValue) const = 0;
	virtual bool IsValidGrid(const FGrid& Grid) const = 0;
	virtual FGrid WorldToGrid(const FVector& WorldPosition) const = 0;
	virtual FVector GridToWorld(const FGrid& Grid) const = 0;
	virtual UGridObject* GetValidGridObject(const FGrid& Grid) const = 0;
	virtual void AddUnitAtGrid(AUnit* Unit, const FGrid& GridValue) = 0;
	virtual void RemoveUnitAtGrid(AUnit* Unit, const FGrid& GridValue) = 0;
	virtual void MoveUnitGrid(AUnit* Unit, const FGrid& From, const FGrid& to) = 0;
	virtual TMap<FGrid, UGridObject*> GetAllGridObjectsThatHasUnit() const = 0;

	/*Pathfinding Functions*/
	virtual TArray<FGrid> FindPath(const FGrid& Start, const FGrid& End, int32& PathLength, const int32 MaxMoveCost, bool bCanIgnoreUnit = false, bool bCalculateToTarget = false) = 0;
	virtual int32 CalculateGridDistance(const FGrid& a, const FGrid& b) const = 0;
	virtual UPathObject* GetLowestFCostObject(TArray<UPathObject*>& PathObjectList) = 0;
	virtual TArray<FGrid> CalculatePath(UPathObject* EndObject) const = 0;
	virtual TArray<UPathObject*> GetNearObjectArray(UPathObject* CurrentObject) const = 0;
	virtual bool HasPath(const FGrid& Start, const FGrid& End, int32 MaxMoveCost = 1000, bool bCanIgnoreUnit = false) = 0;
	virtual void InitAllPathFindingObjects() = 0;
	virtual int32 GetPathLength(const FGrid& Start, const FGrid& End, const int32 MaxMoveCost) = 0;


};
