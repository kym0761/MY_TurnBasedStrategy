// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnitActionComponent.h"
#include "UnitMoveActionComponent.generated.h"

//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStartMoving);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEndMoving);

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TURNBASEDSTRATEGY_API UUnitMoveActionComponent : public UUnitActionComponent
{
	GENERATED_BODY()

public:
		UUnitMoveActionComponent();

		//FOnStartMoving OnStartMoving;
		//FOnEndMoving OnEndMoving;

private:

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


	virtual TArray<FGrid> GetValidActionGridArray() const override;
	virtual TArray<FGridVisualData> GetValidActionGridVisualDataArray() const override;
	virtual void TakeAction(const FGrid& Grid) override;

	virtual void DealWithGridBeforeAction(const FGrid& Grid) override;

	virtual void OnActionStartFunc() override;
	virtual void OnActionEndFunc() override;
	virtual void OnActionSelectedFunc() override;

	virtual FGrid ThinkAIBestActionGrid() override;
	virtual void TestFunction() override;
};
