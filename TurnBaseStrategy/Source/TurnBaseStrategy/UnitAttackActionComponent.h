// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnitActionComponent.h"
#include "UnitAttackActionComponent.generated.h"

/**
 * 
 */
UCLASS()
class TURNBASESTRATEGY_API UUnitAttackActionComponent : public UUnitActionComponent
{
	GENERATED_BODY()
	
public:
	UUnitAttackActionComponent();

public:

	virtual TArray<FGrid> GetValidActionGridArray() const override;
	virtual TArray<FGridVisualData> GetValidActionGridVisualDataArray() const;
	virtual void TakeAction(FGrid Grid) override;
};
