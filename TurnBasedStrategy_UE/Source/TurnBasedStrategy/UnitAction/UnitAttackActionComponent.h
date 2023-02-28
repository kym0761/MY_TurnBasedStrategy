// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnitActionComponent.h"
#include "UnitAttackActionComponent.generated.h"

class UAttackCalculationWidget;

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TURNBASEDSTRATEGY_API UUnitAttackActionComponent : public UUnitActionComponent
{
	GENERATED_BODY()
	
public:
	UUnitAttackActionComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UMG")
		TSubclassOf<UAttackCalculationWidget> AttackCalculationWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "UMG")
		UAttackCalculationWidget* AttackCalculationWidget;

protected:
	virtual void BeginPlay() override;

public:

	virtual void DealWithGridBeforeAction(const FGrid& Grid) override;

	virtual TArray<FGrid> GetValidActionGridArray() const override;
	virtual TArray<FGridVisualData> GetValidActionGridVisualDataArray() const override;
	virtual void TakeAction(const FGrid& Grid) override;

	virtual void OnActionStartFunc() override;
	virtual void OnActionEndFunc() override;
	virtual void OnActionSelectedFunc() override;

	virtual FGrid ThinkAIBestActionGrid() override;
	virtual void TestFunction() override;
};
