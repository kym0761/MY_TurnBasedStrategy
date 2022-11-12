// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnitActionComponent.h"
#include "UnitInteractActionComponent.generated.h"

/**
 * 
 */
UCLASS()
class TURNBASESTRATEGY_API UUnitInteractActionComponent : public UUnitActionComponent
{
	GENERATED_BODY()
	
public:

	UUnitInteractActionComponent();

	virtual TArray<FGrid> GetValidActionGridArray() const override;
	virtual TArray<FGridVisualData> GetValidActionGridVisualDataArray() const;
	virtual void TakeAction(FGrid Grid) override;
};
