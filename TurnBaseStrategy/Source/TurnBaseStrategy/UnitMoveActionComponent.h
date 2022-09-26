// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnitActionComponent.h"
#include "UnitMoveActionComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStartMoving);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEndMoving);

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TURNBASESTRATEGY_API UUnitMoveActionComponent : public UUnitActionComponent
{
	GENERATED_BODY()

public:
		UUnitMoveActionComponent();

		FOnStartMoving OnStartMoving;
		FOnEndMoving OnEndMoving;

protected:

	virtual void BeginPlay() override;

private: 

public:

	virtual FString GetActionName() const override;
	virtual TArray<FGrid> GetValidActionGridArray() const override;
	virtual void TakeAction(FGrid Grid) override;
};
