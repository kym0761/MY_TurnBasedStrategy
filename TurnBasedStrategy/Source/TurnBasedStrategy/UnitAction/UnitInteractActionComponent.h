// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnitActionComponent.h"
#include "UnitInteractActionComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TURNBASEDSTRATEGY_API UUnitInteractActionComponent : public UUnitActionComponent
{
	GENERATED_BODY()
	
public:

	UUnitInteractActionComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual TSet<FGrid> GetValidActionGridSet() const override;
	virtual TSet<FGridVisualData> GetValidActionGridVisualDataSet() const override;
	virtual void TakeAction(const FGrid& Grid) override;
	virtual void DealWithGridBeforeAction(const FGrid& Grid) override;
protected:

	virtual void ActionStart() override;
	virtual void ActionEnd() override;
	virtual void ActionSelected() override;

};
