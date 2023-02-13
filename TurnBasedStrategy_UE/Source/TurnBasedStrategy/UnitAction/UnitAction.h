// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnitAction.generated.h"
/**
 * 
 */

UENUM(BlueprintType)
enum class EUnitActionType : uint8
{
	Move UMETA(DisplayName = "Move"),
	Attack UMETA(DisplayName = "Attack"),
	Interact UMETA(DisplayName = "Interact"),
	Wait UMETA(DisplayName = "Wait")
};
