// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitInteractActionComponent.h"

UUnitInteractActionComponent::UUnitInteractActionComponent()
{
	MaxActionRange = 1;

	ActionName = FString("Interact");
}

void UUnitInteractActionComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UUnitInteractActionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
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
