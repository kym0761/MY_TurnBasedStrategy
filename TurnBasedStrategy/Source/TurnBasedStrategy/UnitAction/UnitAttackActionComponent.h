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

	virtual TSet<FGrid> GetValidActionGridSet() const override;
	virtual TSet<FGridVisualData> GetValidActionGridVisualDataSet() const override;
	virtual void TakeAction(const FGrid& Grid) override;

	virtual FGrid ThinkAIBestActionGrid() override;
	virtual int32 CalculateActionValue(FGrid& CandidateGrid) override;
	virtual void AI_Action() override;

	//적 공격 가능 범위를 표현
	TSet<FGrid> GetEnemyAttackableGridRange();
	//적이 해당 위치에 있다 가정하고 공격할 수 있는 위치인지 확인.
	TSet<FGrid> GetAttackRangeGridSetAtGrid(FGrid& Grid);

protected:
	virtual void ActionStart() override;
	virtual void ActionEnd() override;
	virtual void ActionSelected() override;

};
