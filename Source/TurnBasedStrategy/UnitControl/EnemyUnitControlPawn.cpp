// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyUnitControlPawn.h"
#include "Manager/SRPG_GameMode.h"
#include "UnitCore/Unit.h"
#include "UnitAction/UnitMoveActionComponent.h"
#include "UnitAction/UnitAttackActionComponent.h"

#include "DebugHelper.h"

AEnemyUnitControlPawn::AEnemyUnitControlPawn()
{
	PawnTurnType = ETurnType::EnemyTurn;
}

void AEnemyUnitControlPawn::BeginPlay()
{
	Super::Super::BeginPlay();

}

void AEnemyUnitControlPawn::Tick(float DeltaTime)
{
	Super::Super::Tick(DeltaTime);

}

void AEnemyUnitControlPawn::TriggerToPlay()
{
	Debug::Print(DEBUG_TEXT("TriggerToPlay"));

	//TODO : ����, ������ �׾��� ��Ȳ������ AI Control�� ���������� �������� ����. 
	//����) �÷��̾��� ������ ������ ������ ���� �� �� ������ �׾��� ���, ����2) AI�� ������ ���� �߿� AI�� ������ �׾��� ���. Ȥ�� �÷��̾� ������ �׾��� ���.
	//�ذ�å ���� : ������ ���� ��, �ִϸ��̼� ó�� �� ���� ���� �ð������� Busy ó���Ͽ� AI Control�� ������Ű�� ����� �ʿ��� ��.
	
	FindEnemyAllUnits();
	AIPawnMode = EAIPawnMode::Move; // �� ���۵Ǹ� PawnMode�� Move�� �����.
	DoAIControl();
}

void AEnemyUnitControlPawn::FindEnemyAllUnits()
{
	ASRPG_GameMode* gameMode = ASRPG_GameMode::GetSRPG_GameMode(GetWorld());

	if (!IsValid(gameMode))
	{
		Debug::Print(DEBUG_TEXT("gameMode is Invalid."));
		return;
	}

	auto unitArr = gameMode->GetAllUnitInGridSystem();
	TArray<AUnit*> enemyArr;
	for (auto unit : unitArr)
	{
		if (!IsValid(unit))
		{
			continue;
		}

		if (unit->ActorHasTag(ENEMY))
		{
			enemyArr.Add(unit);
		}
	}

	EnemyUnits.Empty();
	EnemyUnits = enemyArr;

	for (auto unit : EnemyUnits)
	{
		unit->StartUnitTurn();
		TArray<UActorComponent*> unitActions;
		unit->GetComponents(UUnitActionComponent::StaticClass(), unitActions);

		for (UActorComponent* unitAction : unitActions)
		{
			UUnitActionComponent* unitAction_Cast =
				Cast<UUnitActionComponent>(unitAction);

			if (!IsValid(unitAction_Cast))
			{
				continue;
			}

			unitAction_Cast->OnActionCompleteForControlPawn.Clear();
			unitAction_Cast->OnActionCompleteForControlPawn.AddDynamic(this, &AUnitControlPawn::OnUnitActionCompleted);
		}
	}

}

void AEnemyUnitControlPawn::MoveProcedure()
{
	//1 �̵��� ���� ����
	//2 �̵� ������ ����
	//3 �̵� ������ ������ ActionEnd()�ǰ�, ������ �ൿ�� ���ƴٴ� ���� AIControlPawn���� �˸�.

	if (EnemyUnits.Num() == 0)
	{
		//Move�� ������ ���ɼ��� ����
		AIPawnMode = EAIPawnMode::Attack;
		FindEnemyAllUnits(); //���� ������ ������ �ٽ� ã�� �ڿ� ���� Control�� ������.
		DoAIControl();

		return;
	}

	AUnit* currentEnemyUnit = EnemyUnits[0];
	EnemyUnits.RemoveAt(0);

	if (!IsValid(currentEnemyUnit))
	{
		return;
	}

	UUnitMoveActionComponent* unitMoveComp = currentEnemyUnit->FindComponentByClass<UUnitMoveActionComponent>();
	if (!IsValid(unitMoveComp))
	{
		return;
	}

	unitMoveComp->AI_Action();
}

void AEnemyUnitControlPawn::AttackProcedure()
{
	//1. ���� ������ �������� Ȯ��
	//2. ������ or ���� �Ұ����ϸ� �׳� TakeAction��.
	//3. ������ ������ �� ControlPawn���� ������ ���ƴٴ� ���� �˸�.

	if (EnemyUnits.Num() == 0)
	{
		//Attack�� ������ ���ɼ��� ����
		AIPawnMode = EAIPawnMode::Wait;
		DoAIControl();

		return;
	}


	AUnit* currentEnemyUnit = EnemyUnits[0];
	EnemyUnits.RemoveAt(0);

	if (!IsValid(currentEnemyUnit))
	{
		return;
	}

	UUnitAttackActionComponent* unitAttackComp = currentEnemyUnit->FindComponentByClass<UUnitAttackActionComponent>();
	if (!IsValid(unitAttackComp))
	{
		return;
	}

	unitAttackComp->AI_Action();
}

void AEnemyUnitControlPawn::WaitProcedure()
{
	// ���� �������� Wait�� �ʿ� ���� �׳� ���� ������ �� �� ����.

	Debug::Print(DEBUG_TEXT("WaitProcedure"));

	ASRPG_GameMode* gameMode = ASRPG_GameMode::GetSRPG_GameMode(GetWorld());
	if (IsValid(gameMode))
	{
		gameMode->NextTurn();
	}

}

void AEnemyUnitControlPawn::DoAIControl()
{
	switch (AIPawnMode)
	{
	case EAIPawnMode::NONE:
		AIPawnMode = EAIPawnMode::Move;
		MoveProcedure();
		break;
	case EAIPawnMode::Move:
		MoveProcedure();
		break;
	case EAIPawnMode::Attack:
		//AttackProcedure();
		WaitProcedure(); // ������ �ʹ� ���� �ɷ���, ������� ���� �׳� ������ Move�� ������ �������� ���� Wait���� �� �����ϰ� ����.
		break;
	case EAIPawnMode::Wait:
		WaitProcedure();
		break;
	default:
		break;
	}
}

void AEnemyUnitControlPawn::OnUnitActionCompleted()
{
	//����, Unit �ϳ��� �����̴� ���� �����ٸ�
	//���� ������ ������ �� ���� ����.

	DoAIControl();
}