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

	//!! EnemyUnitControlPawn은 사실 어디에 있든 별 상관 없을지도 모른다.
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
	//1 이동할 유닛 선택
	//2 이동 명령을 내림
	//3 이동 끝나면 Call될 명령을 Bind하기
	//4 Bind된 명령 제거해놓기.

	if (EnemyUnits.Num() == 0)
	{
		//Move가 끝났을 가능성이 높음
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
	
	//실질적 Unit Move : Function Name 바꿀 필요있을 듯.
	unitMoveComp->TestFunction();

}

void AEnemyUnitControlPawn::OnUnitMoveFinished()
{
	//만약, Unit 하나가 움직이는 것이 끝났다면
	//다음 유닛을 움직일 수 있을 것임.

	MoveProcedure();
}
