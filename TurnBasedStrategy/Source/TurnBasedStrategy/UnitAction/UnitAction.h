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

//Grid 위치에 대한 Action의 Value를 계산하기 위한 Token
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
