// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnitActionComponent.h"
#include "WaitActionComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TURNBASESTRATEGY_API UWaitActionComponent : public UUnitActionComponent
{
	GENERATED_BODY()
	
public:

	UWaitActionComponent();

protected:

	virtual void BeginPlay() override;

public:

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction);

	virtual void TakeAction(FGrid Grid) override;

	virtual void OnActionSelectedFunc() override;
};
