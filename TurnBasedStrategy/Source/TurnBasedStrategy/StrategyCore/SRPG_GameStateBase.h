// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyDelegates.h"
#include "GameFramework/GameStateBase.h"
#include "Manager/Turn.h"
#include "SRPG_GameStateBase.generated.h"

class AUnit;

/**
 * 
 */
UCLASS()
class TURNBASEDSTRATEGY_API ASRPG_GameStateBase : public AGameStateBase
{
	GENERATED_BODY()
	
public:

	ASRPG_GameStateBase();

	static ASRPG_GameStateBase* GetSRPG_GameState(const UObject* WorldContextObject);

	//Team01의 턴이 시작됐을 때, 유닛들에게 Callback
	FDynamicMulticastVoid Team01Started;

	//Team02의 턴이 시작됐을 때, 유닛들에게 Callback
	FDynamicMulticastVoid Team02Started;
	
	//Team03의 턴이 시작됐을 때, 유닛들에게 Callback
	FDynamicMulticastVoid Team03Started;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Turn", Meta = (AllowPrivateAccess = true))
	int32 TurnNumber = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Turn", Meta = (AllowPrivateAccess = true))
	ETurnType TurnType = ETurnType::Team01Turn;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Turn", Meta = (AllowPrivateAccess = true))
	bool bIsBusy = false;

	UPROPERTY()
	int32 PlayerUnitNumber;

	UPROPERTY()
	int32 EnemyUnitNumber;

	//맵에 존재하지 않으면(예시 : 죽으면) 이 겜의 패배 조건
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Strategy")
	TArray<TSubclassOf<AUnit>> CoreUnitClassArray;

public:

	//유닛 숫자가 줄었을 때,
	//GameState마다 다른 조건을 추가하여 승리 / 패배 조건을 변경하기 위해서 virtual
	virtual void OnUnitNumberChanged();

	bool CheckCurrentTurnValidation();
	void InitializeTurn();

	FORCEINLINE int32 GetTurnNumber() const { return TurnNumber; }
	FORCEINLINE ETurnType GetTurnType() const { return TurnType; }
	FORCEINLINE bool GetIsBusy() const { return bIsBusy; }

	void ToNextTurn();
	void NextTurnNumber();
};
