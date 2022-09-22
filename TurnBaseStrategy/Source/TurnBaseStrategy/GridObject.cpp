// Fill out your copyright notice in the Description page of Project Settings.


#include "GridObject.h"
#include "UnitCharacter.h"

UGridObject::UGridObject()
{
	Grid = FGrid();
}

FString UGridObject::ToString()
{
	return Grid.ToString() + FString(" / ") + GetName();
}

void UGridObject::AddUnit(AUnitCharacter* Unit)
{
	UnitArray.Add(Unit);
}

void UGridObject::RemoveUnit(AUnitCharacter* Unit)
{
	UnitArray.Remove(Unit);
}

bool UGridObject::HasAnyUnit()
{
	return UnitArray.Num() > 0;
}

AUnitCharacter* UGridObject::GetUnit()
{
	if (HasAnyUnit())
	{
		return UnitArray[0];
	}

	return nullptr;
}

void UGridObject::SetGrid(FGrid InGrid)
{
	Grid = InGrid;
}

FGrid UGridObject::GetGrid() const
{
	return Grid;
}
