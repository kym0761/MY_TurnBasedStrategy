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
	//UI를 띄우기.
	//UI에서 Attack OK 명령을 받았을 시에 공격하기.

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

	//!! Widget을 생성하는 것보다, Main Canvas에 넣고 쓰게 만들 예정.

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

			//존재하지 않는 Grid
			if (!gridManager->IsValidGrid(resultGrid))
			{
				continue;
			}

			////지금 현재 Unit의 위치
			if (resultGrid == unitGrid)
			{
				continue;
			}

			//상대가 같은 팀 tag가 붙어있으면 스킵.
			AUnitCharacter* targetUnit = gridManager->GetUnitAtGrid(resultGrid);
			if (!IsValid(targetUnit) || GetOwner()->Tags.Num() > 0 && targetUnit->ActorHasTag(GetOwner()->Tags[0]))
			{
				continue;
			}

			//통과하면 문제없으니 validArray에 추가
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

			//존재하지 않는 Grid
			if (!gridManager->IsValidGrid(resultGrid))
			{
				continue;
			}

			FGridVisualData resultData;
			resultData.Grid = resultGrid;
			resultData.GridVisualType = EGridVisualType::NO;

			////지금 현재 Unit의 위치
			if (resultGrid == unitGrid)
			{
				continue;
			}

			//상대가 같은 팀 tag가 붙어있으면 스킵.
			AUnitCharacter* targetUnit = gridManager->GetUnitAtGrid(resultGrid);
			if (!IsValid(targetUnit) || GetOwner()->Tags.Num() > 0 && targetUnit->ActorHasTag(GetOwner()->Tags[0]))
			{
				continue;
			}

			//통과하면 문제없으니 validArray에 추가

			validVisualDataArray.Add(resultData);
		}
	}


	return validVisualDataArray;



}

void UUnitAttackActionComponent::TakeAction(const FGrid& Grid)
{
	//TArray<FGrid> tempArr = GetValidActionGridArray();
	//

	////TODO : 데미지 주는 Animation을 Play & 데미지 처리

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
	TArray<FGrid> grids = GetValidActionGridArray(); //공격할 수 있는 위치 전부.
	TArray<FActionValueToken> actionValues;

	AGridManager* gridManager = AGridManager::GetGridManager();
	if (!IsValid(gridManager))
	{
		//불가.
		return FGrid(-1, -1);
	}

	AAttackManager* attackManager = AAttackManager::GetAttackManager();
	if (!IsValid(attackManager))
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
	AAttackManager* attackManager = AAttackManager::GetAttackManager();
	if (!IsValid(attackManager))
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

			//존재하지 않는 Grid
			if (!gridManager->IsValidGrid(resultGrid))
			{
				continue;
			}

			//통과하면 문제없으니 validArray에 추가
			validArray.Add(resultGrid);
		}
	}

	validArray.AddUnique(Unit->GetGrid());

	return validArray;
}