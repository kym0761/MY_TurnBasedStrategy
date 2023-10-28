// Fill out your copyright notice in the Description page of Project Settings.


#include "SRPG_GameMode.h"

#include "Kismet/GameplayStatics.h"

#include "DebugHelper.h"

#include "UnitCore/Unit.h"
#include "UnitAction/UnitActionComponent.h"
#include "UnitCore/StatComponent.h"

ASRPG_GameMode::ASRPG_GameMode()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ASRPG_GameMode::BeginPlay()
{
	Super::BeginPlay();

	//DebugHelper.h에 존재하는 디버그 활성/ 비활성 Flag다.
	DebugControl::bOK_Debugging = bUseCustomDebug;


	TArray<AActor*> units;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AUnit::StaticClass(), units);

	for (auto i : units)
	{
		auto unit = Cast<AUnit>(i);

		if (IsValid(unit))
		{
			//유닛의 모든 Action에 ActionEnd가 끝나면 Game Win or Lose를 확인하도록 연결한다.

			auto unitActions = unit->K2_GetComponentsByClass(UUnitActionComponent::StaticClass());

			for (auto j : unitActions)
			{
				auto unitAction = CastChecked<UUnitActionComponent>(j);
				if (IsValid(unitAction))
				{
					unitAction->OnActionEnd.AddDynamic(this, &ASRPG_GameMode::CheckWinOrLose);
				}
			}

			//유닛 죽었을 때도 Game Win Lose 체크
			unit->OnUnitDestroyed.AddDynamic(this, &ASRPG_GameMode::CheckWinOrLose);

		}

	}

}

void ASRPG_GameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

}

void ASRPG_GameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

ASRPG_GameMode* ASRPG_GameMode::GetSRPG_GameMode()
{

	if (!IsValid(GEngine) || !IsValid(GEngine->GameViewport))
	{
		return nullptr;
	}

	UWorld* world = GEngine->GameViewport->GetWorld();

	ASRPG_GameMode* gameMode = Cast<ASRPG_GameMode>(UGameplayStatics::GetActorOfClass(world, ASRPG_GameMode::StaticClass()));

	return gameMode;
}

void ASRPG_GameMode::CheckWinOrLose()
{
	TArray<AActor*> units;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AUnit::StaticClass(), units);

	int32 team01Count = 0;
	int32 team02Count = 0;

	for (auto i : units)
	{
		auto unit = Cast<AUnit>(i);

		if (IsValid(unit))
		{
			if (unit->StatComponent->GetHP() <= 0.0f)
			{
				continue;
			}

			if (unit->GetTeamType() == ETeamType::Team01)
			{
				team01Count++;
			}
			else if (unit->GetTeamType() == ETeamType::Team02)
			{
				team02Count++;
			}

		}
	}

	if (team01Count == 0)
	{
		LoseGame();
	}
	else if (team02Count == 0)
	{
		WinGame();
	}
	else
	{
		Debug::Print(DTEXT_ARGS(TEXT("Team01 : %d, Team02 : %d"), team01Count, team02Count));
	}

}

void ASRPG_GameMode::WinGame()
{
	Debug::Print(DEBUG_TEXT("WIN GAME"));
}

void ASRPG_GameMode::LoseGame()
{
	Debug::Print(DEBUG_TEXT("LOSE GAME"));
}