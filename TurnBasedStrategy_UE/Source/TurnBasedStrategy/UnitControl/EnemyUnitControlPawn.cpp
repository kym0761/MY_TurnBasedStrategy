// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyUnitControlPawn.h"
#include "Manager/SRPG_GameMode.h"
#include "UnitCore/Unit.h"
#include "UnitAction/UnitMoveActionComponent.h"
#include "UnitAction/UnitAttackActionComponent.h"

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
	UE_LOG(LogTemp, Warning, TEXT("AEnemyUnitControlPawn::TriggerToPlay()"));

	//TODO : 현재, 유닛이 죽었을 상황에서는 AI Control이 정상적으로 동작하지 않음. 
	//예시) 플레이어의 마지막 유닛이 공격을 했을 때 적 유닛이 죽었을 경우, 예시2) AI의 유닛이 공격 중에 AI의 유닛이 죽었을 경우. 혹은 플레이어 유닛이 죽었을 경우.
	//해결책 예상 : 유닛이 죽을 때, 애니메이션 처리 및 유닛 삭제 시간까지는 Busy 처리하여 AI Control을 지연시키는 기능이 필요할 것.
	
	FindEnemyAllUnits();
	AIPawnMode = EAIPawnMode::Move; // 턴 시작되면 PawnMode가 Move로 변경됨.
	DoAIControl();
}

void AEnemyUnitControlPawn::FindEnemyAllUnits()
{
	ASRPG_GameMode* gameMode = ASRPG_GameMode::GetSRPG_GameMode(GetWorld());

	if (!IsValid(gameMode))
	{
		UE_LOG(LogTemp, Warning, TEXT("gameMode is not Valid"));
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
	//1 이동할 유닛 선택
	//2 이동 명령을 내림
	//3 이동 명령이 끝나면 ActionEnd()되고, 유닛의 행동을 마쳤다는 것을 AIControlPawn에게 알림.

	if (EnemyUnits.Num() == 0)
	{
		//Move가 끝났을 가능성이 높음
		AIPawnMode = EAIPawnMode::Attack;
		FindEnemyAllUnits(); //공격 가능한 유닛을 다시 찾은 뒤에 다음 Control을 실행함.
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
	//1. 공격 가능한 유닛인지 확인
	//2. 공격함 or 공격 불가능하면 그냥 TakeAction함.
	//3. 공격이 끝났을 때 ControlPawn에게 공격을 마쳤다는 것을 알림.

	if (EnemyUnits.Num() == 0)
	{
		//Attack이 끝났을 가능성이 높음
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
	// 유닛 각각에게 Wait할 필요 없이 그냥 턴을 끝내면 될 것 같다.

	UE_LOG(LogTemp, Warning, TEXT("AEnemyUnitControlPawn::WaitProcedure()"));

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
		WaitProcedure(); // 공격이 너무 오래 걸려서, 디버깅을 위해 그냥 유닛이 Move를 끝내면 공격하지 말고 Wait으로 턴 종료하게 만듬.
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
	//만약, Unit 하나가 움직이는 것이 끝났다면
	//다음 유닛을 움직일 수 있을 것임.

	DoAIControl();
}
