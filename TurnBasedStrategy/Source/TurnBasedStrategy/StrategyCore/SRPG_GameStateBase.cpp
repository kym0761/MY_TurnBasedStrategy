// Fill out your copyright notice in the Description page of Project Settings.


#include "StrategyCore/SRPG_GameStateBase.h"

#include "Kismet/GameplayStatics.h"
#include "UnitCore/Unit.h"

#include "DebugHelper.h"


ASRPG_GameStateBase::ASRPG_GameStateBase()
{
}

ASRPG_GameStateBase* ASRPG_GameStateBase::GetSRPG_GameState(const UObject* WorldContextObject)
{
	ASRPG_GameStateBase* gameState = nullptr;

	if (IsValid(WorldContextObject))
	{
		gameState = Cast<ASRPG_GameStateBase>(UGameplayStatics::GetGameState(WorldContextObject));
	}

	return gameState;
}

void ASRPG_GameStateBase::OnUnitNumberChanged()
{
	{
		for (auto coreUnitClass : CoreUnitClassArray)
		{
			TArray<AActor*> outUnit;

			UGameplayStatics::GetAllActorsOfClass(this, coreUnitClass, outUnit);

			if (outUnit.Num() == 0)
			{
				//gamemode call lose

				return;
			}

		}
	}

	{
		TArray<AActor*> outUnit;
		UGameplayStatics::GetAllActorsOfClass(this, AUnit::StaticClass(), outUnit);


		PlayerUnitNumber = 0;
		for (auto unit : outUnit)
		{
			//플레이어의 unit인지 확인하여 count함.
			PlayerUnitNumber++;
		}

		if (PlayerUnitNumber == 0)
		{
			// gamemode call lose
			return;
		}
	}

	{
		TArray<AActor*> outUnit;
		UGameplayStatics::GetAllActorsOfClass(this, AUnit::StaticClass(), outUnit);
		

		EnemyUnitNumber = 0;
		for (auto unit : outUnit)
		{
			//enemy인지 확인할 수 있는 조건 상태에 EnemyNum을 count함.
			EnemyUnitNumber++;
		}

		if (EnemyUnitNumber == 0)
		{
			// gamemode call win
			return;
		}
	}
	

}

bool ASRPG_GameStateBase::CheckCurrentTurnValidation()
{
	//각 유닛이 행동이 가능한지 가능하지 않은지를 확인해서
	//가능한 유닛이 있으면 턴을 true
	//행동 가능한 유닛이 없으면 false한다.

	bool isTurnValid = false;

	TArray<AActor*> outUnit;
	UGameplayStatics::GetAllActorsOfClass(this, AUnit::StaticClass(), outUnit);

	ETeamType teamCompare = ETeamType::Team01;

	switch (TurnType)
	{
	case ETurnType::Team01Turn:
		teamCompare = ETeamType::Team01;
		break;
	case ETurnType::Team02Turn:
		teamCompare = ETeamType::Team02;
		break;
	case ETurnType::Team03Turn:
		teamCompare = ETeamType::Team03;
		break;
	default:
		break;
	}

	for (auto actor : outUnit)
	{
		AUnit* unit = Cast<AUnit>(actor);

		if (!IsValid(unit))
		{
			continue;
		}

		if (unit->GetTeamType() != teamCompare)
		{
			continue;
		}

		if (unit->IsThisUnitCanAction())
		{
			isTurnValid = true;
			break;
		}
	}

	return isTurnValid;


}

void ASRPG_GameStateBase::InitializeTurn()
{
	TurnNumber = 1;

}
