// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitAttackActionComponent.h"
#include "Manager/GridManager.h"
#include "UMG/AttackCalculationWidget.h"
#include "Manager/AttackManager.h"
#include "UnitCore/StatComponent.h"
#include "UnitCore/UnitCharacter.h"
#include "UnitMoveActionComponent.h"

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

	AGridManager* gridManager = AGridManager::GetGridManager();
	if (!IsValid(gridManager))
	{
		UE_LOG(LogTemp, Warning, TEXT("Grid Manager is not Valid"));
		return;
	}

	TArray<FGrid> validAttackRange = GetValidActionGridArray();
	if (!validAttackRange.Contains(Grid))
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't Attack."));
		return;
	}

	AUnitCharacter* unit = gridManager->GetUnitAtGrid(Grid);
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

	//!! Widget�� �����ϴ� �ͺ���, Main Canvas�� �ְ� ���� ���� ����.

	AttackCalculationWidget = CreateWidget<UAttackCalculationWidget>(GetWorld(), AttackCalculationWidgetClass);
	if (IsValid(AttackCalculationWidget))
	{
		AttackCalculationWidget->AddToViewport();
		AttackCalculationWidget->
			InitAttackCalculationWidget(GetOwner(), gridManager->GetUnitAtGrid(Grid));
	}

}

TArray<FGrid> UUnitAttackActionComponent::GetValidActionGridArray() const
{
	TArray<FGrid> validArray;

	FGrid unitGrid = Unit->GetGrid();

	AGridManager* gridManager = AGridManager::GetGridManager();

	if (!IsValid(gridManager))
	{
		UE_LOG(LogTemp, Warning, TEXT("Grid Manager is not Valid"));
		return validArray;
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
			if (!gridManager->IsValidGrid(resultGrid))
			{
				continue;
			}

			////���� ���� Unit�� ��ġ
			if (resultGrid == unitGrid)
			{
				continue;
			}

			//��밡 ���� �� tag�� �پ������� ��ŵ.
			AUnitCharacter* targetUnit = gridManager->GetUnitAtGrid(resultGrid);
			if (!IsValid(targetUnit) || GetOwner()->Tags.Num() > 0 && targetUnit->ActorHasTag(GetOwner()->Tags[0]))
			{
				continue;
			}

			//����ϸ� ���������� validArray�� �߰�
			validArray.Add(resultGrid);
		}
	}



	return validArray;
}

TArray<FGridVisualData> UUnitAttackActionComponent::GetValidActionGridVisualDataArray() const
{
	TArray<FGridVisualData> validVisualDataArray;
	FGrid unitGrid = Unit->GetGrid();

	AGridManager* gridManager = AGridManager::GetGridManager();

	if (!IsValid(gridManager))
	{
		UE_LOG(LogTemp, Warning, TEXT("Grid Manager is not Valid"));
		return validVisualDataArray;
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
			if (!gridManager->IsValidGrid(resultGrid))
			{
				continue;
			}

			FGridVisualData resultData;
			resultData.Grid = resultGrid;
			resultData.GridVisualType = EGridVisualType::NO;

			////���� ���� Unit�� ��ġ
			if (resultGrid == unitGrid)
			{
				continue;
			}

			//��밡 ���� �� tag�� �پ������� ��ŵ.
			AUnitCharacter* targetUnit = gridManager->GetUnitAtGrid(resultGrid);
			if (!IsValid(targetUnit) || GetOwner()->Tags.Num() > 0 && targetUnit->ActorHasTag(GetOwner()->Tags[0]))
			{
				continue;
			}

			//����ϸ� ���������� validArray�� �߰�

			validVisualDataArray.Add(resultData);
		}
	}


	return validVisualDataArray;



}

void UUnitAttackActionComponent::TakeAction(const FGrid& Grid)
{
	//TArray<FGrid> tempArr = GetValidActionGridArray();
	//

	////TODO : ������ �ִ� Animation�� Play & ������ ó��

	//if (tempArr.Contains(Grid))
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("Attack At : %s"), *Grid.ToString());
	//}
	//else
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("You Selected which is Not Valid Grid. ---> Grid Pos : %s"), *Grid.ToString());
	//}

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

	Super::ActionEnd();

	auto owner = Cast<AUnitCharacter>(GetOwner());
	if (IsValid(owner))
	{
		owner->FinishUnitAllAction();
	}
}

void UUnitAttackActionComponent::ActionSelected()
{
	Super::ActionSelected();
}

FGrid UUnitAttackActionComponent::ThinkAIBestActionGrid()
{
	TArray<FGrid> grids = GetValidActionGridArray(); //������ �� �ִ� ��ġ ����.
	TArray<FActionValueToken> actionValues;

	AGridManager* gridManager = AGridManager::GetGridManager();
	if (!IsValid(gridManager))
	{
		//�Ұ�.
		return FGrid(-1, -1);
	}

	AAttackManager* attackManager = AAttackManager::GetAttackManager();
	if (!IsValid(attackManager))
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
	AActor* attacker = GetOwner();
	if (!IsValid(attacker) || attacker->Tags.Num() == 0)
	{
		return -1;
	}

	AGridManager* gridManager = AGridManager::GetGridManager();
	if (!IsValid(gridManager))
	{
		return -1;
	}

	AAttackManager* attackManager = AAttackManager::GetAttackManager();
	if (!IsValid(attackManager))
	{
		return -1;
	}

	auto defender = gridManager->GetUnitAtGrid(CandidateGrid);
	if (!IsValid(defender))
	{
		return -1;
	}

	TArray<FAttackOrder> attackOrders = attackManager->CalculateAttackOrder(attacker, defender);

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

void UUnitAttackActionComponent::TestFunction()
{
	AAttackManager* attackManager = AAttackManager::GetAttackManager();
	if (!IsValid(attackManager))
	{
		//�Ұ�.
		return;
	}

	FGrid grid = ThinkAIBestActionGrid();

	AGridManager* gridManager = AGridManager::GetGridManager();
	if (!IsValid(gridManager))
	{
		//�Ұ�.
		return;
	}

	attackManager->SetupAttackManager(GetOwner(),gridManager->GetUnitAtGrid(grid));
	attackManager->StartAttack();

}

TArray<FGrid> UUnitAttackActionComponent::GetEnemyAttackableGridRange()
{
	UUnitMoveActionComponent* moveActionComp = GetOwner()->FindComponentByClass<UUnitMoveActionComponent>();
	if (!IsValid( moveActionComp))
	{
		return TArray<FGrid>();
	}

	TArray<FGrid> resultGrid;

	TArray<FGrid> canMoveGrids = moveActionComp->GetValidActionGridArray();

	for (FGrid& canMoveGrid : canMoveGrids)
	{
		TArray<FGrid> grids = GetAttackRangeGridArrayAtGrid(canMoveGrid);

		for (FGrid& grid : grids)
		{
			resultGrid.AddUnique(grid);
		}

	}

	return resultGrid;

}

TArray<FGrid> UUnitAttackActionComponent::GetAttackRangeGridArrayAtGrid(FGrid& Grid)
{
	TArray<FGrid> validArray;

	AGridManager* gridManager = AGridManager::GetGridManager();

	if (!IsValid(gridManager))
	{
		UE_LOG(LogTemp, Warning, TEXT("Grid Manager is not Valid"));
		return validArray;
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
			if (!gridManager->IsValidGrid(resultGrid))
			{
				continue;
			}

			//����ϸ� ���������� validArray�� �߰�
			validArray.Add(resultGrid);
		}
	}

	validArray.AddUnique(Unit->GetGrid());

	return validArray;
}