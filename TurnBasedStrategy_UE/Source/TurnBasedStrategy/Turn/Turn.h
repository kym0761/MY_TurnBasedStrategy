// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class ETurnType : uint8
{
	PlayerTurn UMETA(DisplayName = "PlayerTurn"),
	EnemyTurn UMETA(DisplayName = "EnemyTurn"),
	AllyTurn UMETA(DisplayName = "AllyTurn")

};