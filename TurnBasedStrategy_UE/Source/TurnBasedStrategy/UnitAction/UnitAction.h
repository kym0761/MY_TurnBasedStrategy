// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Grid/Grid.h"
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

USTRUCT(BlueprintType, Blueprintable)
struct FActionValueToken
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
		FGrid Grid;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
		int32 ActionValue;

	FActionValueToken();

};
