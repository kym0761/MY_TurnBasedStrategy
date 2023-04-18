// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitAttackActionComponent.h"
#include "UMG/AttackCalculationWidget.h"
#include "UnitCore/StatComponent.h"
#include "UnitCore/Unit.h"
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
	//UI�� ����.
	//UI���� Attack OK ����� �޾��� �ÿ� �����ϱ�.

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

	AUnit* unit = gameMode->GetUnitAtGrid(Grid);
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

	//TODO : Widget�� �����ϴ� �ͺ���, Main Canvas�� �ְ� Show / Hide �ϴ°� �� ���ƺ���. ���� �����ؾ���
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
	//���� ������ ��� Grid ��ġ�� ���� ã�� return�Ѵ�.

	TSet<FGrid> validSet;

	AUnit* unit = GetOwningUnit();
	if (!IsValid(unit))
	{
		return validSet;
	}

	FGrid unitGrid = unit->GetGrid();

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

			//�������� �ʴ� Grid
			if (!gameMode->IsValidGrid(resultGrid))
			{
				continue;
			}

			////���� ���� Unit�� ��ġ
			if (resultGrid == unitGrid)
			{
				continue;
			}

			//��밡 ���� �� tag�� �پ������� ��ŵ.
			AUnit* targetUnit = gameMode->GetUnitAtGrid(resultGrid);
			if (!IsValid(targetUnit) || GetOwner()->Tags.Num() > 0 && 
				targetUnit->ActorHasTag(GetOwner()->Tags[0]))
			{
				continue;
			}

			//����ϸ� ���������� validSet�� �߰�
			validSet.Add(resultGrid);
		}
	}

	return validSet;
}

TSet<FGridVisualData> UUnitAttackActionComponent::GetValidActionGridVisualDataSet() const
{
	//�� Function�� Visual Data ����

	TSet<FGrid> grids = GetValidActionGridSet();
	TSet<FGridVisualData> validVisualDataSet;

	for (FGrid& grid : grids)
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
	//Attack Action�� TakeAction�� AttackManager���� ����ȣ���.
	//AI�� ���, ������ ������ ��ġ�� ���ٸ�, AI_Action���� TakeAction�� �Ͽ� ������ ������.

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

	//������ ��ģ ������ ���� �ٸ� �ൿ �Ұ�.
	AUnit* owner = GetOwningUnit();
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
	//���� ���� ������ ������ ���� �� ���� ��ġ�� ���� �ൿ�� ������.
	//����) ���� ����� ���� �Ǹ� ���� ���� ���� �� ���� �� �ش� Action�� ������.

	TSet<FGrid> grids = GetValidActionGridSet(); //������ �� �ִ� ��ġ ����.
	TArray<FActionValueToken> actionValues;

	AGridManager* gridManager = AGridManager::GetGridManager();
	if (!IsValid(gridManager))
	{
		//�Ұ�.
		return FGrid(-1, -1);
	}

	ASRPG_GameMode* gameMode = ASRPG_GameMode::GetSRPG_GameMode(GetWorld());
	if (!IsValid(gameMode))
	{
		//�Ұ�.
		return FGrid(-1, -1);
	}

	//���� ������ ��ġ ���� Ȯ���ؼ� �ش� ��ġ�� Value�� ���.
	for (FGrid& grid : grids)
	{
		FActionValueToken actionValueToken;
		actionValueToken.Grid = grid;
		actionValueToken.ActionValue = CalculateActionValue(grid);

		//Action Value�� ���̳ʽ���, �ش� ��ġ�� �����Ϸ��� �õ��� ������ ��.
		if (actionValueToken.ActionValue == -1)
		{
			continue;
		}

		actionValues.Add(actionValueToken);
	}

	if (actionValues.Num() == 0)
	{
		//Ȯ���� Grid�� ����.
		return FGrid(-1, -1);
	}

	Algo::Sort(actionValues, [](FActionValueToken& a, FActionValueToken& b)
		{
			return a.ActionValue > b.ActionValue;
		});

	//���� Value�� ���ؼ�, ���� ���� ���� (���� ��ġ�� ���� ����) Grid�� ����.
	FActionValueToken selectedActionValueToken = actionValues[0];

	return selectedActionValueToken.Grid;
}

int32 UUnitAttackActionComponent::CalculateActionValue(FGrid& CandidateGrid)
{
	//AttackOrder�� �޾� ����� �м���.
	//����� �� �ǰ� ���� ���� ����, ��� �ǰ� ���� ���� ���� ��ġ�� ����

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

	//AttackerHP�� �������� ������ ����. DefenderHP�� �������� ������ ����.
	float valueScore = 0.0f;

	float counterAttackValue = 100 * attackerHP / attackerStatComponent->GetMaxHP();
	valueScore += FMath::Clamp<int32>(counterAttackValue, 0.0f, 100.0f);

	float attackValue = 100 * (defenderStatComponent->GetMaxHP() - defenderHP) / defenderStatComponent->GetMaxHP();
	valueScore += FMath::Clamp<int32>(attackValue, 0.0f, 100.0f);

	return (int32)valueScore;
}

void UUnitAttackActionComponent::AI_Action()
{
	ASRPG_GameMode* gameMode = ASRPG_GameMode::GetSRPG_GameMode(GetWorld());
	if (!IsValid(gameMode))
	{
		//�Ұ�.
		return;
	}

	FGrid grid = ThinkAIBestActionGrid();

	//������ �Ұ����ϸ� takeactionó���Ͽ� ���� �ൿ�� ������.
	if (grid == FGrid(-1, -1))
	{
		TakeAction(grid);
	}

	gameMode->SetupAttackManaging(GetOwner(), gameMode->GetUnitAtGrid(grid));
	gameMode->StartAttack();
}

TSet<FGrid> UUnitAttackActionComponent::GetEnemyAttackableGridRange()
{
	//�� ������ �̵��Ÿ� + ���� ���� ���� �ջ��Ͽ� ���� ������ ����� �� �����.

	UUnitMoveActionComponent* moveActionComp = GetOwner()->FindComponentByClass<UUnitMoveActionComponent>();
	if (!IsValid( moveActionComp))
	{
		return TSet<FGrid>();
	}

	TSet<FGrid> resultGrid;

	TSet<FGrid> canMoveGrids = moveActionComp->GetValidActionGridSet();

	for (FGrid& canMoveGrid : canMoveGrids)
	{
		//�� ��ġ���� ���� ������ Grid ��ġ�� �˻�
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
	//Grid ��ġ���� Attack Range ��ŭ ���� ������ Grid�� ����.

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

			//�������� �ʴ� Grid
			if (!gameMode->IsValidGrid(resultGrid))
			{
				continue;
			}

			//����ϸ� ���������� validSet�� �߰�
			validSet.Add(resultGrid);
		}
	}

	return validSet;
}