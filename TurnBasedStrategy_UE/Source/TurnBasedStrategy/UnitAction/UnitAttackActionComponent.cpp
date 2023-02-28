// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitAttackActionComponent.h"
#include "UnitCore/UnitCharacter.h"
#include "Manager/GridManager.h"
#include "UMG/AttackCalculationWidget.h"
#include "Manager/AttackManager.h"

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
			FGrid resultGrid = FGrid(x, y);
			resultGrid += unitGrid;

			if (FMath::Abs(x) + FMath::Abs(y) > MaxActionRange)
			{
				continue;
			}

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

	if (OnActionEnd.IsBound())
	{
		OnActionEnd.Broadcast();
	}

	
}

void UUnitAttackActionComponent::OnActionStartFunc()
{
	Super::OnActionStartFunc();
}

void UUnitAttackActionComponent::OnActionEndFunc()
{
	Super::OnActionEndFunc();


	AGridManager* gridManager = AGridManager::GetGridManager();

	if (!IsValid(gridManager))
	{
		UE_LOG(LogTemp, Warning, TEXT("Grid Manager is not Valid"));
		return;
	}

	gridManager->RemoveAllGridVisual();

}

void UUnitAttackActionComponent::OnActionSelectedFunc()
{
	Super::OnActionSelectedFunc();
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
		actionValueToken.ActionValue = attackManager->CalculateGridValue_ToAttack(GetOwner(), gridManager->GetUnitAtGrid(grid));

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
