// Fill out your copyright notice in the Description page of Project Settings.


#include "GridObject.h"
#include "../UnitCharacter.h"
#include "GridSystem.h"

UGridObject::UGridObject()
{
	Grid = FGrid();
}

FString UGridObject::ToString() const
{
	FString str;

	str += Grid.ToString() + FString(" / ") + GetName();

	for (int32 i = 0; i < UnitArray.Num(); i++)
	{
		str += FString(" / ") + UnitArray[i]->GetName();
	}

	return str;
}

void UGridObject::AddUnit(AUnitCharacter* Unit)
{
	if (!IsValid(Unit))
	{
		return;
	}

	UnitArray.Add(Unit);
}

void UGridObject::RemoveUnit(AUnitCharacter* Unit)
{
	if (UnitArray.Contains(Unit))
	{
		UnitArray.Remove(Unit);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Unit is not inside"));
	}
	
}

bool UGridObject::HasAnyUnit() const
{
	return UnitArray.Num() > 0;
}

AUnitCharacter* UGridObject::GetUnit() const
{
	if (HasAnyUnit())
	{
		return UnitArray[0];
	}

	return nullptr;
}

TArray<AUnitCharacter*> UGridObject::GetUnitArray() const
{
	return UnitArray;
}

void UGridObject::SetGrid(FGrid InGrid)
{
	Grid = InGrid;
}

FGrid UGridObject::GetGrid() const
{
	return Grid;
}

UGridSystem* UGridObject::GetGridSystem() const
{
	return GridSystem;
}

void UGridObject::SetGridSystem(UGridSystem* InGridSystem)
{
	GridSystem = InGridSystem;
}
