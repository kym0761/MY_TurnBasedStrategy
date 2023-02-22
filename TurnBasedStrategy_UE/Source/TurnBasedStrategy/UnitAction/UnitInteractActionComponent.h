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

	virtual TArray<FGrid> GetValidActionGridArray() const override;
	virtual TArray<FGridVisualData> GetValidActionGridVisualDataArray() const override;
	virtual void TakeAction(const FGrid& Grid) override;


	virtual void OnActionStartFunc() override;
	virtual void OnActionEndFunc() override;
	virtual void OnActionSelectedFunc() override;

};
