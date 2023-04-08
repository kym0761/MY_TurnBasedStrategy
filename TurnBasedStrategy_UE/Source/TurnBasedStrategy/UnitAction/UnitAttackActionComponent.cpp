// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitAttackActionComponent.h"
#include "UMG/AttackCalculationWidget.h"
#include "UnitCore/StatComponent.h"
#include "UnitCore/UnitCharacter.h"
#include "UnitMoveActionComponent.h"
#include "Manager/SRPG_GameMode.h"
#include "Manager/GridManager.h"

UUnitAttackActionComponent::UUnitAttackActionComponent()
{
    MaxActionRange = 1;

	ActionName = FString("Attack");
}

void UUnitAttackActionComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UUnitAttackActionComponent::DealWithGridBeforeAction(const FGrid& Grid)
{
	//UI를 띄우기.
	//UI에서 Attack OK 명령을 받았을 시에 공격하기.

	if (!IsValid(AttackCalculationWidgetClass))
	{
		UE_LOG(LogTemp, Warning, TEXT("AttackCalculationWidgetClass is not Valid"));
		return;
	}


	ASRPG_GameMode* gameMode = ASRPG_GameMode::GetSRPG_GameMode(GetWorld());
	if (!IsValid(gameMode))
	{
		UE_LOG(LogTemp, Warning, TEXT("gameMode is not Valid"));
		return;
	}

	TSet<FGrid> validAttackRange = GetValidActionGridSet();
	if (!validAttackRange.Contains(Grid))
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't Attack."));
		return;
	}

	AUnitCharacter* unit = gameMode->GetUnitAtGrid(Grid);
	if (!IsValid(unit))
	{
		UE_LOG(LogTemp, Warning, TEXT("there is no unit at you selected."));
		return;
	}

	if (unit == GetOwner())
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't Attack Yourself..."));
		return;
	}

	//TODO : Widget을 생성하는 것보다, Main Canvas에 넣고 Show / Hide 하는게 더 나아보임. 추후 수정해야함
	AttackCalculationWidget = CreateWidget<UAttackCalculationWidget>(GetWorld(), AttackCalculationWidgetClass);
	if (IsValid(AttackCalculationWidget))
	{
		AttackCalculationWidget->AddToViewport();
		AttackCalculationWidget->
			InitAttackCalculationWidget(GetOwner(), gameMode->GetUnitAtGrid(Grid));
	}

}

TSet<FGrid> UUnitAttackActionComponent::GetValidActionGridSet() const
{
	//공격 범위에 닿는 Grid 위치를 전부 찾아 return한다.

	TSet<FGrid> validSet;

	FGrid unitGrid = Unit->GetGrid();

	ASRPG_GameMode* gameMode = ASRPG_GameMode::GetSRPG_GameMode(GetWorld());

	if (!IsValid(gameMode))
	{
		UE_LOG(LogTemp, Warning, TEXT("gameMode is not Valid"));
		return validSet;
	}

	for (int x = -MaxActionRange; x <= MaxActionRange; x++)
	{
		for (int y = -MaxActionRange; y <= MaxActionRange; y++)
		{
			if (FMath::Abs(x) + FMath::Abs(y) > MaxActionRange)
			{
				continue;
			}

			FGrid resultGrid = FGrid(x, y);
			resultGrid += unitGrid;

			//존재하지 않는 Grid
			if (!gameMode->IsValidGrid(resultGrid))
			{
				continue;
			}

			////지금 현재 Unit의 위치
			if (resultGrid == unitGrid)
			{
				continue;
			}

			//상대가 같은 팀 tag가 붙어있으면 스킵.
			AUnitCharacter* targetUnit = gameMode->GetUnitAtGrid(resultGrid);
			if (!IsValid(targetUnit) || GetOwner()->Tags.Num() > 0 && targetUnit->ActorHasTag(GetOwner()->Tags[0]))
			{
				continue;
			}

			//통과하면 문제없으니 validSet에 추가
			validSet.Add(resultGrid);
		}
	}

	return validSet;
}

TSet<FGridVisualData> UUnitAttackActionComponent::GetValidActionGridVisualDataSet() const
{
	//위 Function의 Visual Data 버전

	auto grids = GetValidActionGridSet();
	TSet<FGridVisualData> validVisualDataSet;

	for (auto grid : grids)
	{
		FGridVisualData resultData;
		resultData.Grid = grid;
		resultData.GridVisualType = EGridVisualType::NO;

		validVisualDataSet.Add(resultData);
	}

	return validVisualDataSet;
}

void UUnitAttackActionComponent::TakeAction(const FGrid& Grid)
{
	//Attack Action의 TakeAction은 AttackManager에서 최종호출됨.

	ActionEnd();	
}

void UUnitAttackActionComponent::ActionStart()
{
	Super::ActionStart();
}

void UUnitAttackActionComponent::ActionEnd()
{
	AGridManager* gridManager = AGridManager::GetGridManager();

	if (!IsValid(gridManager))
	{
		UE_LOG(LogTemp, Warning, TEXT("Grid Manager is not Valid"));
		return;
	}

	gridManager->RemoveAllGridVisual();

	//공격을 마친 유닛은 이후 다른 행동 불가.
	auto owner = Cast<AUnitCharacter>(GetOwner());
	if (IsValid(owner))
	{
		owner->FinishUnitAllAction();
	}

	Super::ActionEnd();
}

void UUnitAttackActionComponent::ActionSelected()
{
	Super::ActionSelected();
}

FGrid UUnitAttackActionComponent::ThinkAIBestActionGrid()
{
	//공격 범위 내에서 가능한 공격 중 제일 가치가 높은 행동을 선택함.
	//예시) 공격 결과로 적의 피를 가장 많이 낮출 수 있을 때 해당 Action을 선택함.

	TSet<FGrid> grids = GetValidActionGridSet(); //공격할 수 있는 위치 전부.
	TArray<FActionValueToken> actionValues;

	AGridManager* gridManager = AGridManager::GetGridManager();
	if (!IsValid(gridManager))
	{
		//불가.
		return FGrid(-1, -1);
	}

	ASRPG_GameMode* gameMode = ASRPG_GameMode::GetSRPG_GameMode(GetWorld());
	if (!IsValid(gameMode))
	{
		//불가.
		return FGrid(-1, -1);
	}

	//공격 가능한 위치 전부 확인해서 해당 위치의 Value를 계산.
	for (FGrid& grid : grids)
	{
		FActionValueToken actionValueToken;
		actionValueToken.Grid = grid;
		actionValueToken.ActionValue = CalculateActionValue(grid);

		actionValues.Add(actionValueToken);
	}

	if (actionValues.Num() == 0)
	{
		//확인할 Grid가 없음.
		return FGrid(-1, -1);
	}

	Algo::Sort(actionValues, [](FActionValueToken& a, FActionValueToken& b)
		{
			return a.ActionValue > b.ActionValue;
		});

	//계산된 Value에 대해서, 가장 값이 높은 (공격 가치가 제일 높은) Grid를 선택.
	FActionValueToken selectedActionValueToken = actionValues[0];

	return selectedActionValueToken.Grid;
}

int32 UUnitAttackActionComponent::CalculateActionValue(FGrid& CandidateGrid)
{
	//AttackOrder를 받아 결과를 분석함.
	//결과로 내 피가 가장 높을 수록, 상대 피가 가장 적을 수록 가치가 높음

	AGridManager* gridManager = AGridManager::GetGridManager();
	if (!IsValid(gridManager))
	{
		return -1;
	}

	ASRPG_GameMode* gameMode = ASRPG_GameMode::GetSRPG_GameMode(GetWorld());
	if (!IsValid(gameMode))
	{
		return -1;
	}

	AActor* attacker = GetOwner();
	if (!IsValid(attacker) || attacker->Tags.Num() == 0)
	{
		return -1;
	}

	auto defender = gameMode->GetUnitAtGrid(CandidateGrid);
	if (!IsValid(defender))
	{
		return -1;
	}

	TArray<FAttackOrder> attackOrders = gameMode->CalculateAttackOrder(attacker, defender);

	UStatComponent* attackerStatComponent =
		attacker->FindComponentByClass<UStatComponent>();

	UStatComponent* defenderStatComponent =
		defender->FindComponentByClass<UStatComponent>();

	if (!IsValid(attackerStatComponent) || !IsValid(defenderStatComponent))
	{
		return -1;
	}

	float attackerHP = attackerStatComponent->GetHP();
	float defenderHP = defenderStatComponent->GetHP();

	for (auto attackOrder : attackOrders)
	{
		switch (attackOrder.AttackOrderType)
		{
		case EAttackOrderType::Attack:
			attackerHP -= attackOrder.Damage;
			break;
		case EAttackOrderType::Defend:
			defenderHP -= attackOrder.Damage;
			break;
		}
	}

	//AttackerHP가 많을수록 점수가 높음. DefenderHP가 적을수록 점수가 높음.
	float valueScore = 0.0f;

	float counterAttackValue = 100 * attackerHP / attackerStatComponent->GetMaxHP();
	valueScore += FMath::Clamp<int32>(counterAttackValue, 0.0f, 100.0f);

	float attackValue = 100 * (defenderStatComponent->GetMaxHP() - defenderHP) / defenderStatComponent->GetMaxHP();
	valueScore += FMath::Clamp<int32>(attackValue, 0.0f, 100.0f);

	return (int32)valueScore;
}

void UUnitAttackActionComponent::TestFunction()
{
	ASRPG_GameMode* gameMode = ASRPG_GameMode::GetSRPG_GameMode(GetWorld());
	if (!IsValid(gameMode))
	{
		//불가.
		return;
	}

	FGrid grid = ThinkAIBestActionGrid();

	AGridManager* gridManager = AGridManager::GetGridManager();
	if (!IsValid(gridManager))
	{
		//불가.
		return;
	}

	gameMode->SetupAttackManaging(GetOwner(), gameMode->GetUnitAtGrid(grid));
	gameMode->StartAttack();
}

TSet<FGrid> UUnitAttackActionComponent::GetEnemyAttackableGridRange()
{
	//적 유닛의 이동거리 + 공격 범위 전부 합산하여 위험 지역을 계산할 때 사용함.

	UUnitMoveActionComponent* moveActionComp = GetOwner()->FindComponentByClass<UUnitMoveActionComponent>();
	if (!IsValid( moveActionComp))
	{
		return TSet<FGrid>();
	}

	TSet<FGrid> resultGrid;

	TSet<FGrid> canMoveGrids = moveActionComp->GetValidActionGridSet();

	for (FGrid& canMoveGrid : canMoveGrids)
	{
		//이 위치에서 공격 가능한 Grid 위치를 검색
		TSet<FGrid> grids = GetAttackRangeGridSetAtGrid(canMoveGrid);

		for (FGrid& grid : grids)
		{
			resultGrid.Add(grid);
		}

	}

	return resultGrid;
}

TSet<FGrid> UUnitAttackActionComponent::GetAttackRangeGridSetAtGrid(FGrid& Grid)
{
	//Grid 위치에서 Attack Range 만큼 공격 가능한 Grid를 도출.

	TSet<FGrid> validSet;

	ASRPG_GameMode* gameMode = ASRPG_GameMode::GetSRPG_GameMode(GetWorld());

	if (!IsValid(gameMode))
	{
		UE_LOG(LogTemp, Warning, TEXT("gameMode is not Valid"));
		return validSet;
	}

	for (int x = -MaxActionRange; x <= MaxActionRange; x++)
	{
		for (int y = -MaxActionRange; y <= MaxActionRange; y++)
		{
			if (FMath::Abs(x) + FMath::Abs(y) > MaxActionRange)
			{
				continue;
			}

			FGrid resultGrid = FGrid(x, y);
			resultGrid += Grid;

			//존재하지 않는 Grid
			if (!gameMode->IsValidGrid(resultGrid))
			{
				continue;
			}

			//통과하면 문제없으니 validSet에 추가
			validSet.Add(resultGrid);
		}
	}

	return validSet;
}