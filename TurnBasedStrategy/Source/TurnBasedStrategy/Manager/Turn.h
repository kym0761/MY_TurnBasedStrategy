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
	Team01Turn UMETA(DisplayName = "Team01Turn"), //플레이어 턴
	Team02Turn UMETA(DisplayName = "Team02Turn"), // 적 턴
	Team03Turn UMETA(DisplayName = "Team03Turn") // 중립
};

UENUM()
enum class ETeamType : uint8
{
	Team01 UMETA(DisplayName = "Team01"), //Player
	Team02 UMETA(DisplayName = "Team02"), //Enemy
	Team03 UMETA(DisplayName = "Team03") // 중립
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
		float CritRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Order")
		float Accuracy;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Order")
		AActor* Attacker;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Order")
		AActor* Defender;

};

UENUM(BlueprintType)
enum class EOrderOwnerType : uint8
{
	Attacker UMETA(DisplayName = "Attacker"),
	Defender UMETA(DisplayName = "Defender")
};

USTRUCT(BlueprintType, Blueprintable)
struct FBattleOrder
{
	GENERATED_BODY()

public:

	FBattleOrder();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Order")
	EOrderOwnerType OrderOwnerType;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Order")
	int32 Damage;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Order")
	float CritRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Order")
	float Accuracy;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Order")
	AActor* Attacker;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Order")
	AActor* Defender;

};