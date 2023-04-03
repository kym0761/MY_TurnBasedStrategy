// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnitActionComponent.h"
#include "UnitMoveActionComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TURNBASEDSTRATEGY_API UUnitMoveActionComponent : public UUnitActionComponent
{
	GENERATED_BODY()

public:
		UUnitMoveActionComponent();

private:

		//tick에서 필요한 정보들
		UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
		FGrid Destination;
		UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
		bool bMoveActivate = false;
		UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
		TArray<FGrid> Path;

		//Tick Logic
		bool bIsMoving = false;

protected:

	virtual void BeginPlay() override;

public:

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction);


	virtual TSet<FGrid> GetValidActionGridSet() const override;
	virtual TSet<FGridVisualData> GetValidActionGridVisualDataSet() const override;
	virtual void TakeAction(const FGrid& Grid) override;

	virtual void DealWithGridBeforeAction(const FGrid& Grid) override;

	virtual FGrid ThinkAIBestActionGrid() override;
	virtual int32 CalculateActionValue(FGrid& CandidateGrid) override;
	virtual void TestFunction() override;

protected:
	
	virtual void ActionStart() override;
	virtual void ActionEnd() override;
	virtual void ActionSelected() override;

};
