// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyUnitControlPawn.h"
#include "Manager/GridManager.h"
#include "UnitCore/UnitCharacter.h"
#include "UnitAction/UnitMoveActionComponent.h"
#include "UnitAction/UnitAttackActionComponent.h"

AEnemyUnitControlPawn::AEnemyUnitControlPawn()
{
	PawnTurnType = ETurnType::EnemyTurn;
}

void AEnemyUnitControlPawn::BeginPlay()
{
	Super::Super::BeginPlay();

	//!! EnemyUnitControlPawn�� ��� ��� �ֵ� �� ��� �������� �𸥴�.
	InitGridPosition(FGrid(0,0));

}

void AEnemyUnitControlPawn::Tick(float DeltaTime)
{
	Super::Super::Tick(DeltaTime);

}

void AEnemyUnitControlPawn::FindEnemyAllUnits()
{
	AGridManager* gridManager = AGridManager::GetGridManager();

	if (!IsValid(gridManager))
	{
		UE_LOG(LogTemp, Warning, TEXT("Grid Manager is not Valid"));
		return;
	}

	auto unitArr = gridManager->GetAllUnitInGridSystem();
	TArray<AUnitCharacter*> enemyArr;
	for (auto unit : unitArr)
	{
		if (!IsValid(unit))
		{
			continue;
		}

		if (unit->ActorHasTag(TEXT("Enemy")))
		{
			enemyArr.Add(unit);
		}
	}

	EnemyUnits.Empty();
	EnemyUnits = enemyArr;
}

void AEnemyUnitControlPawn::MoveProcedure()
{
	//1 �̵��� ���� ����
	//2 �̵� ����� ����
	//3 �̵� ������ Call�� ����� Bind�ϱ�
	//4 Bind�� ��� �����س���.

	if (EnemyUnits.Num() == 0)
	{
		//Move�� ������ ���ɼ��� ����
		return;
	}

	auto currentEnemyUnit = EnemyUnits[0];
	if (!IsValid(currentEnemyUnit))
	{
		return;
	}

	auto unitMoveComp = currentEnemyUnit->FindComponentByClass<UUnitMoveActionComponent>();
	if (!IsValid(unitMoveComp))
	{
		return;
	}

	//unitMoveComp->OnActionEnd.AddDynamic(this, &AEnemyUnitControlPawn::OnUnitMoveFinished);
	
	//������ Unit Move : Function Name �ٲ� �ʿ����� ��.
	unitMoveComp->TestFunction();

}

void AEnemyUnitControlPawn::OnUnitMoveFinished()
{
	//����, Unit �ϳ��� �����̴� ���� �����ٸ�
	//���� ������ ������ �� ���� ����.

	MoveProcedure();
}
