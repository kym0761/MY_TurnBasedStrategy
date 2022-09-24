// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnitActionComponent.h"
#include "UnitMoveActionComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TURNBASESTRATEGY_API UUnitMoveActionComponent : public UUnitActionComponent
{
	GENERATED_BODY()

public:
		UUnitMoveActionComponent();
	
protected:

private: 

public:

	virtual FString GetActionName() const override;
	virtual TArray<FGrid> GetValidActionGridArray() const override;
	virtual void TakeAction(FGrid Grid) override;
};
