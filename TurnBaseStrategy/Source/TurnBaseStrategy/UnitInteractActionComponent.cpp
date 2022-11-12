// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitInteractActionComponent.h"

UUnitInteractActionComponent::UUnitInteractActionComponent()
{
	MaxActionRange = 1;

	ActionName = FString("Interact");
}

TArray<FGrid> UUnitInteractActionComponent::GetValidActionGridArray() const
{
	return TArray<FGrid>();
}

TArray<FGridVisualData> UUnitInteractActionComponent::GetValidActionGridVisualDataArray() const
{
	return TArray<FGridVisualData>();
}

void UUnitInteractActionComponent::TakeAction(FGrid Grid)
{
}
