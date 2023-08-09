// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnitActionComponent.h"
#include "UnitInventoryActionComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TURNBASEDSTRATEGY_API UUnitInventoryActionComponent : public UUnitActionComponent
{
	GENERATED_BODY()

public:

	UUnitInventoryActionComponent();

};
