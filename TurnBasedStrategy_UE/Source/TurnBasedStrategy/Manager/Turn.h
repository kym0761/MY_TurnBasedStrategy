// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Turn.generated.h"
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

UENUM(BlueprintType)
enum class EAttackOrderType : uint8
{
	Attack UMETA(DisplayName = "Attack"),

	Defend UMETA(DisplayName = "Defend")

};

USTRUCT(BlueprintType, Blueprintable)
struct FAttackOrder
{
	GENERATED_BODY()

public:

	FAttackOrder();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Order")
		EAttackOrderType AttackOrderType;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Order")
		int32 Damage;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Order")
		AActor* Attacker;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Order")
		AActor* Defender;

};

//UENUM(meta = (Bitflags))
//enum class EBitmaskExample : uint8
//{
//	A,
//	B,
//	C
//};