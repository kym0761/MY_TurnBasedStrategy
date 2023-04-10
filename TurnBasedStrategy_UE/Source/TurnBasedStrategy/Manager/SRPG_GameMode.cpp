// Fill out your copyright notice in the Description page of Project Settings.


#include "SRPG_GameMode.h"
#include "Kismet/GameplayStatics.h"
#include "UnitCore/UnitCharacter.h"
#include "UnitControl/EnemyUnitControlPawn.h"
#include "UnitControl/UnitControlPawn.h"

#include "UnitCore/UnitAnimInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "UnitAction/UnitAttackActionComponent.h"
#include "UnitCore/StatComponent.h"
#include "UnitCore/UnitCharacter.h"

#include "Grid/GridObject.h"
#include "Grid/PathNode.h"
#include "Grid/PathFindingSystem.h"
#include "Grid/GridSystem.h"
#include "GridManager.h"


ASRPG_GameMode::ASRPG_GameMode()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ASRPG_GameMode::BeginPlay()
{
	Super::BeginPlay();

	//UE_LOG(LogTemp, Warning, TEXT("ASRPG_GameMode::beginPlay()"));

}

void ASRPG_GameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options,ErrorMessage);

}

void ASRPG_GameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

ASRPG_GameMode* ASRPG_GameMode::GetSRPG_GameMode(const UObject* WorldContextObject)
{
	ASRPG_GameMode* gameMode = nullptr;

	if (IsValid(WorldContextObject))
	{
		gameMode = Cast<ASRPG_GameMode>(UGameplayStatics::GetGameMode(WorldContextObject));
	}

	return gameMode;
}

void ASRPG_GameMode::InitTurn()
{
	// 모든 유닛을 찾고, 유닛이 Player의 유닛인지, 아니면 Enemy인지 확인해서 분류함.
	// 마지막엔 UnitControlPawn에게도 해당 유닛이 각각 본인들의 것이라는 것을 알려줌.

	TArray<AActor*> unitArr;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AUnitCharacter::StaticClass(), unitArr);

	for (auto actor : unitArr)
	{
		AUnitCharacter* unit = Cast<AUnitCharacter>(actor);
		if (IsValid(unit))
		{
			AddUnitForTurnManaging(unit);
		}
	}

	EnemyUnitControlPawnRef = Cast<AEnemyUnitControlPawn>(UGameplayStatics::GetActorOfClass(GetWorld(), AEnemyUnitControlPawn::StaticClass()));

	TArray<AActor*> controlPawnArr;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AUnitControlPawn::StaticClass(), controlPawnArr);
	for (auto controlpawn : controlPawnArr)
	{
		auto controlPawn_Casted = Cast<AUnitControlPawn>(controlpawn);
		if (!IsValid(controlPawn_Casted))
		{
			continue;
		}
		if (controlpawn != EnemyUnitControlPawnRef)
		{
			PlayerUnitControlPawnRef = controlPawn_Casted;
		}
	}

	PlayerUnitControlPawnRef->FindAllPlayerUnits();
	EnemyUnitControlPawnRef->FindEnemyAllUnits();
}

void ASRPG_GameMode::NextTurn()
{
	switch (TurnType)
	{
	case ETurnType::PlayerTurn:
		SetTurnType(ETurnType::EnemyTurn);
		break;
	case ETurnType::EnemyTurn:
		//SetTurnType(ETurnType::AllyTurn);
		////!! Ally를 만들게 되면 이 부분을 변경할 것.
		SetTurnType(ETurnType::PlayerTurn);
		NextTurnNumber();
		break;
	//case ETurnType::AllyTurn:
	//	SetTurnType(ETurnType::PlayerTurn);
	//	NextTurnNumber();
	//	break;
	}

}

void ASRPG_GameMode::ResetTurn()
{
	TurnNumber = 1;
	TurnType = ETurnType::PlayerTurn;
}

void ASRPG_GameMode::CheckCurrentTurnValidation()
{
	bool IsTurnValid = false;

	switch (TurnType)
	{
	case ETurnType::PlayerTurn:

		for (auto playerUnit : PlayerUnitArr)
		{
			if (!IsValid(playerUnit))
			{
				continue;
			}

			if (playerUnit->IsThisUnitCanAction())
			{
				IsTurnValid = true;
				break;
			}
		}

		break;
	case ETurnType::EnemyTurn:

		for (auto enemyUnit : EnemyUnitArr)
		{
			if (!IsValid(enemyUnit))
			{
				continue;
			}

			if (enemyUnit->IsThisUnitCanAction())
			{
				IsTurnValid = true;
				break;
			}
		}

		break;
	case ETurnType::AllyTurn:
		break;
	default:
		break;
	}

	if (IsTurnValid == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("Turn Will be Changed."));
		NextTurn();
	}
}

void ASRPG_GameMode::SetTurnType(ETurnType TurnTypeInput)
{
	TurnType = TurnTypeInput;

	switch (TurnType)
	{
	case ETurnType::PlayerTurn:

		PlayerUnitControlPawnRef->SetBusyOrNot(false);

		if (OnPlayerTurnStart.IsBound())
		{
			OnPlayerTurnStart.Broadcast();
		}
		break;
	case ETurnType::EnemyTurn:
		if (OnEnemyTurnStart.IsBound())
		{
			OnEnemyTurnStart.Broadcast();
		}

		if (IsValid(EnemyUnitControlPawnRef))
		{
			EnemyUnitControlPawnRef->TriggerToPlay();
		}

		break;
	case ETurnType::AllyTurn:
		if (OnAllyTurnStart.IsBound())
		{
			OnAllyTurnStart.Broadcast();
		}
		break;
	default:
		break;
	}
}

void ASRPG_GameMode::NextTurnNumber()
{
	TurnNumber++;
}

void ASRPG_GameMode::StartGame()
{
	SetTurnType(ETurnType::PlayerTurn);
	TurnNumber = 1;
}

void ASRPG_GameMode::AddUnitForTurnManaging(AUnitCharacter* UnitToAdd)
{
	if (!IsValid(UnitToAdd))
	{
		//Unit Is Not Valid.
		return;
	}

	if (UnitToAdd->Tags.Num() == 0)
	{
		//Tag is wrong.
		return;
	}

	if (UnitToAdd->ActorHasTag( FName("MyUnit")))
	{
		OnPlayerTurnStart.AddDynamic(UnitToAdd, &AUnitCharacter::StartUnitTurn);
		PlayerUnitArr.Add(UnitToAdd);
	}
	else if (UnitToAdd->ActorHasTag(FName("Enemy")))
	{
		OnEnemyTurnStart.AddDynamic(UnitToAdd, &AUnitCharacter::StartUnitTurn);
		EnemyUnitArr.Add(UnitToAdd);
	}

	UnitToAdd->OnFinishAllAction.AddDynamic(this, &ASRPG_GameMode::CheckCurrentTurnValidation);
}

void ASRPG_GameMode::RemoveUnitFromTurnManaging(AUnitCharacter* UnitToRemove)
{
	if (!IsValid(UnitToRemove))
	{
		//Unit Is Not Valid.
		return;
	}

	if (UnitToRemove->Tags.Num() == 0)
	{
		//Tag is wrong.
		return;
	}

	FName unitTag = UnitToRemove->Tags[0];

	if (unitTag == FName("MyUnit"))
	{
		OnPlayerTurnStart.RemoveDynamic(UnitToRemove, &AUnitCharacter::StartUnitTurn);
		PlayerUnitArr.Remove(UnitToRemove);
	}
	else if (unitTag == FName("Enemy"))
	{
		OnEnemyTurnStart.RemoveDynamic(UnitToRemove, &AUnitCharacter::StartUnitTurn);
		EnemyUnitArr.Remove(UnitToRemove);
	}

	UnitToRemove->OnFinishAllAction.RemoveDynamic(this, &ASRPG_GameMode::CheckCurrentTurnValidation);
}

void ASRPG_GameMode::SetupAttackManaging(AActor* Attacker, AActor* Defender)
{
	if (!IsValid(Attacker) || !IsValid(Defender))
	{
		return;
	}

	OrderToPlay.Empty();
	OrderToPlay = CalculateAttackOrder(Attacker, Defender);
}

void ASRPG_GameMode::StartAttack()
{
	if (OrderToPlay.Num() == 0)
	{
		FinishAttack();
		return;
	}
	FAttackOrder& currentOrder = OrderToPlay[0];

	AActor* currentAttacker = currentOrder.Attacker;
	AActor* currentDefender = currentOrder.Defender;

	if (!IsValid(currentAttacker) || !IsValid(currentDefender))
	{
		FinishAttack();
		return;
	}

	CurrentAttackActionComponent = currentAttacker->FindComponentByClass<UUnitAttackActionComponent>();

	if (!IsValid(CurrentAttackActionComponent))
	{
		//!!경고 필요.
	}


	if (IsValid(currentAttacker))
	{
		FRotator attackerLookRot = UKismetMathLibrary::FindLookAtRotation(currentAttacker->GetActorLocation(), currentDefender->GetActorLocation());
		currentAttacker->SetActorRotation(attackerLookRot);

	}

	if (IsValid(currentDefender))
	{
		FRotator defenderLookRot = UKismetMathLibrary::FindLookAtRotation(currentDefender->GetActorLocation(), currentAttacker->GetActorLocation());
		currentDefender->SetActorRotation(defenderLookRot);
	}

	if (OrderToPlay.Num() > 0)
	{
		PlayAttack();
	}
}

void ASRPG_GameMode::PlayAttack()
{
	FAttackOrder& currentOrder = OrderToPlay[0];

	AActor* currentAttacker = currentOrder.Attacker;
	AActor* currentDefender = currentOrder.Defender;

	if (IsValid(currentAttacker) && IsValid(currentDefender))
	{
		USkeletalMeshComponent* attackerMesh = currentAttacker->FindComponentByClass<USkeletalMeshComponent>();
		USkeletalMeshComponent* defenderMesh = currentDefender->FindComponentByClass<USkeletalMeshComponent>();

		if (IsValid(attackerMesh) && IsValid(defenderMesh))
		{
			UUnitAnimInstance* attackerAnim = Cast<UUnitAnimInstance>(attackerMesh->GetAnimInstance());
			UUnitAnimInstance* defenderAnim = Cast<UUnitAnimInstance>(defenderMesh->GetAnimInstance());

			if (IsValid(attackerAnim) && IsValid(defenderAnim))
			{
				BindOnAttackHit(attackerAnim);
				BindOnAttackEnd(attackerAnim);
				BindOnHit(defenderAnim);
				BindOnHitEnd(defenderAnim);

				bAttackerWaiting = false;
				bDefenderWaiting = false;

				//UE_LOG(LogTemp, Warning, TEXT("Play Montage?"));
				attackerAnim->PlayUnitAttackMontage();
			}
		}
	}
}

void ASRPG_GameMode::FinishAttack()
{
	if (IsValid(CurrentAttackActionComponent))
	{
		CurrentAttackActionComponent->TakeAction(FGrid(0, 0));
	}

	//reset
	bAttackerWaiting = false;
	bDefenderWaiting = false;
	CurrentAttackOrder = FAttackOrder();
}

void ASRPG_GameMode::OnAttackHit()
{
	//Attacker의 공격을 맞았다면 Defender의 hit 애니메이션 재생함.

	FAttackOrder currentOrder = OrderToPlay[0];

	if (!IsValid(currentOrder.Defender) || !IsValid(currentOrder.Attacker))
	{
		return;
	}

	//크리티컬 예시
	float randCrit = FMath::FRandRange(0.0f, 1.0f);
	if (currentOrder.CritRate >= randCrit)
	{
		currentOrder.Damage *= 2;
	}

	float randAccuracy = FMath::FRandRange(0.0f,1.0f);
	if (currentOrder.Accuracy >= randAccuracy)
	{
		UGameplayStatics::ApplyDamage(currentOrder.Defender, currentOrder.Damage, nullptr, currentOrder.Attacker, UDamageType::StaticClass());
	}
	else{
	//빗나간 경우 처리하기?

	}

	USkeletalMeshComponent* defenderMesh = currentOrder.Defender->FindComponentByClass<USkeletalMeshComponent>();
	if (!IsValid(defenderMesh))
	{
		UE_LOG(LogTemp, Warning, TEXT("ASRPG_GameMode::OnAttackHit()  Defender Mesh is Invalid."));
		return;
	}

	UUnitAnimInstance* defenderAnim = Cast<UUnitAnimInstance>(defenderMesh->GetAnimInstance());
	if (IsValid(defenderAnim))
	{
		//UE_LOG(LogTemp, Warning, TEXT("Play Hit Montage?"));
		if (currentOrder.Accuracy >= randAccuracy)
		{
			defenderAnim->PlayUnitHitMontage();
		}
		else
		{
			defenderAnim->PlayUnitAvoidMontage();
		}
	}
}

void ASRPG_GameMode::OnAttackEnd()
{
	bAttackerWaiting = true;
	UE_LOG(LogTemp, Warning, TEXT("AAttackManager::OnAttackEnd()"));
	TryPlayNextOrder();
}

void ASRPG_GameMode::OnHit()
{
}

void ASRPG_GameMode::OnHitEnd()
{
	bDefenderWaiting = true;
	UE_LOG(LogTemp, Warning, TEXT("AAttackManager::OnHitEnd()"));
	TryPlayNextOrder();
}

TArray<FAttackOrder> ASRPG_GameMode::CalculateAttackOrder(AActor* Attacker, AActor* Defender)
{
	// 공격 순서는 1 공격자 2 방어자
	// 공격 속도 차이에 따라 공격자와 방어자가 추가 공격을 행할 수가 있음.
	// 추후 스킬 존재 유무에 따라 공격자와 방어자의 초기 순서와 공격 횟수가 바뀔 수도 있음.

	if (!IsValid(Attacker) || !IsValid(Defender))
	{
		UE_LOG(LogTemp, Warning, TEXT("Attacker or Defender is Not Valid - AAttackManager::CalculateAttackOrder()"));
		return attackOrders;
	}

	UStatComponent* attackerStatComponent =
		Attacker->FindComponentByClass<UStatComponent>();
	UStatComponent* defenderStatComponent =
		Defender->FindComponentByClass<UStatComponent>();

	if (!IsValid(attackerStatComponent) || !IsValid(defenderStatComponent))
	{
		UE_LOG(LogTemp, Warning, TEXT("StatComponents are Not Valid - AAttackManager::CalculateAttackOrder()"));

		return attackOrders;
	}

	int32 attackerDamage = attackerStatComponent->GetSTR();
	int32 attackerSpeed = attackerStatComponent->GetSPD();
	int32 attackerDEF = attackerStatComponent->GetDEF();
	int32 attackerSKL = attackerStatComponent->GetSKL();
	int32 attackerLUK = attackerStatComponent->GetLUK();

	int32 defenderDamage = defenderStatComponent->GetSTR();
	int32 defenderSpeed = defenderStatComponent->GetSPD();
	int32 defenderDEF = defenderStatComponent->GetDEF();
	int32 defenderSKL = defenderStatComponent->GetSKL();
	int32 defenderLUK = defenderStatComponent->GetLUK();

	FAttackOrder attack;
	attack.AttackOrderType = EAttackOrderType::Attack;
	attack.Damage = attackerDamage - defenderDEF;
	attack.Attacker = Attacker;
	attack.Defender = Defender;
	attack.Accuracy = CalculateAccuracy(Attacker, Defender);
	attack.CritRate = CalculateCriticalRate(Attacker, Defender);

	FAttackOrder counterAttack;
	counterAttack.AttackOrderType = EAttackOrderType::Defend;
	counterAttack.Damage = defenderDamage - defenderDEF;
	counterAttack.Attacker = Defender;
	counterAttack.Defender = Attacker;
	counterAttack.Accuracy = CalculateAccuracy(Defender, Attacker);
	counterAttack.CritRate = CalculateCriticalRate(Defender, Attacker);

	TArray<FAttackOrder> attackOrders;
	attackOrders.Add(attack);
	attackOrders.Add(counterAttack);

	//Attacker의 스피드가 5보다 더 높으면 공격을 추가함.
	if (attackerSpeed > defenderSpeed + 5)
	{
		attackOrders.Add(attack);
	}

	//Defender의 스피드가 5보다 더 높으면 공격을 추가함.
	if (attackerSpeed +5 < defenderSpeed)
	{
		attackOrders.Add(counterAttack);
	}

	//Attacker의 스피드가 10보다 더 높으면 추가로 1/2의 데미지만큼 공격을 추가함.
	if (attackerSpeed > defenderSpeed + 10)
	{
		FAttackOrder addAttack;
		attack.AttackOrderType = EAttackOrderType::Attack;
		addAttack.Damage = attack.Damage / 2;
		addAttack.Attacker = Attacker;
		addAttack.Defender = Defender;
		addAttack.Accuracy = attack.Accuracy;
		addAttack.CritRate = attack.CritRate;
		attackOrders.Add(addAttack);
	}

	//defender의 스피드가 10보다 더 높으면 추가로 1/2의 데미지만큼 공격을 추가함.
	if (attackerSpeed +10 < defenderSpeed)
	{
		FAttackOrder addAttack;
		attack.AttackOrderType = EAttackOrderType::Defend;
		addAttack.Damage = counterAttack.Damage / 2;
		addAttack.Attacker = Defender;
		addAttack.Defender = Attacker;
		addAttack.Accuracy = counterAttack.Accuracy;
		addAttack.CritRate = counterAttack.CritRate;
		attackOrders.Add(addAttack);
	}

	return 	attackOrders;
}

void ASRPG_GameMode::BindOnAttackHit(UUnitAnimInstance* AnimInst)
{
	if (!IsValid(AnimInst))
	{
		return;
	}

	FScriptDelegate toBind;
	toBind.BindUFunction(this, FName("OnAttackHit"));

	AnimInst->BindTo_OnAttackHit(toBind);
}

void ASRPG_GameMode::BindOnAttackEnd(UUnitAnimInstance* AnimInst)
{
	if (!IsValid(AnimInst))
	{
		return;
	}

	FScriptDelegate toBind;
	toBind.BindUFunction(this, FName("OnAttackEnd"));

	AnimInst->BindTo_OnAttackEnd(toBind);
}

void ASRPG_GameMode::BindOnHit(UUnitAnimInstance* AnimInst)
{
	if (!IsValid(AnimInst))
	{
		return;
	}

	FScriptDelegate toBind;
	toBind.BindUFunction(this, FName("OnHit"));

	AnimInst->BindTo_OnHit(toBind);
}

void ASRPG_GameMode::BindOnHitEnd(UUnitAnimInstance* AnimInst)
{
	if (!IsValid(AnimInst))
	{
		return;
	}

	FScriptDelegate toBind;
	toBind.BindUFunction(this, FName("OnHitEnd"));

	AnimInst->BindTo_OnHitEnd(toBind);
}

float ASRPG_GameMode::CalculateCriticalRate(AActor* Attacker, AActor* Defender)
{
	//critRate 0.0f~1.0f 사이 값.

	UStatComponent* attackerStatComponent =
		Attacker->FindComponentByClass<UStatComponent>();

	UStatComponent* defenderStatComponent =
		Defender->FindComponentByClass<UStatComponent>();

	if (!(IsValid(attackerStatComponent) && IsValid(defenderStatComponent)))
	{
		//불가능한 크리티컬 확률임.
		return -1.0f;
	}

	int32 attackerSKL = attackerStatComponent->GetSKL();
	int32 attackerLUK = attackerStatComponent->GetLUK();

	int32 defenderSKL = defenderStatComponent->GetSKL();
	int32 defenderLUK = defenderStatComponent->GetLUK();

	//크리 공식 = (공격자 Skill - 방어자 Skill) + (공격자 운 - 방어자 운) / 100.0 + 공격자 SKill + 공격자 운
	float critRate =
		(
			FMath::Clamp(attackerSKL - defenderSKL, 0, attackerSKL)
			+ FMath::Clamp(attackerLUK - defenderLUK, 0, attackerLUK)
			)
		/
		(
			100.0f + attackerSKL + attackerLUK
			);

	return critRate;
}

float ASRPG_GameMode::CalculateAccuracy(AActor* Attacker, AActor* Defender)
{
	//명중률은 0~1 사이 값임.
	//TODO : 명중률은 보여주기만 되는 값이고 현재는 전투 결과에 영향가지 않음.
	//			공격에 명중률을 적용해서 회피도 되게 만들어야함.


	UStatComponent* attackerStatComponent =
		Attacker->FindComponentByClass<UStatComponent>();

	UStatComponent* defenderStatComponent =
		Defender->FindComponentByClass<UStatComponent>();

	if (!(IsValid(attackerStatComponent) && IsValid(defenderStatComponent)))
	{
		//불가능한 명중률임.
		return -1.0f;
	}

	int32 attackerSpeed = attackerStatComponent->GetSPD();
	int32 attackerSKL = attackerStatComponent->GetSKL();
	int32 attackerLUK = attackerStatComponent->GetLUK();

	int32 defenderSpeed = defenderStatComponent->GetSPD();
	int32 defenderSKL = defenderStatComponent->GetSKL();
	int32 defenderLUK = defenderStatComponent->GetLUK();


	//명중률 보정값 + 스킬 차이 + 스피드 차이 + 공격자의 행운 / 100
	//보정값은 게임 난이도에 따라 변경될 수도 있을 듯?
	float correction = 80.0f;
	float accuracy = (correction + (attackerSKL - defenderSKL) + (attackerSpeed - defenderSpeed) + attackerLUK) / 100.0f;

	//명중률은 0~1사이로 Clamp
	accuracy = FMath::Clamp(accuracy, 0.0f, 1.0f);

	return accuracy;
}

void ASRPG_GameMode::TryPlayNextOrder()
{
	if (bAttackerWaiting && bDefenderWaiting)
	{
		//Remove Finished Order
		if (OrderToPlay.Num() > 0)
		{
			OrderToPlay.RemoveAt(0);
		}

		//누군가 죽었으면 이제 Attack 하지 않음.
		if (OrderToPlay.Num() > 0)
		{
			FAttackOrder& currentOrder = OrderToPlay[0];

			if (IsValid(currentOrder.Defender) && IsValid(currentOrder.Attacker))
			{
				UStatComponent* defenderStat = currentOrder.Defender->FindComponentByClass<UStatComponent>();
				UStatComponent* attackerStat = currentOrder.Attacker->FindComponentByClass<UStatComponent>();

				if (IsValid(defenderStat) && IsValid(attackerStat))
				{
					if (defenderStat->GetHP() <= 0.0f || attackerStat->GetHP() <= 0.0f)
					{
						OrderToPlay.Empty();
					}
				}
			}
		}


		if (OrderToPlay.Num() > 0)
		{
			PlayAttack();
		}
		else
		{
			FinishAttack();
		}
	}
}

TArray<FAttackOrder> ASRPG_GameMode::GetAttackOrder() const
{
	return OrderToPlay;
}

void ASRPG_GameMode::CreateGridSystem()
{
	if (IsValid(GridSystem))
	{
		GridSystem = nullptr;
	}
	GridSystem = NewObject<UGridSystem>();

	GridSystem->SetGridSystem(
		X_Length,
		Y_Length,
		[](UGridSystem* gs, FGrid grid) {
			UGridObject* gridObj = NewObject<UGridObject>();
			gridObj->SetGrid(grid);
			gridObj->SetGridSystem(gs);
			return gridObj;
		});

	if (IsValid(PathFindingSystem))
	{
		PathFindingSystem = nullptr;
	}

	PathFindingSystem = NewObject<UPathFindingSystem>();

	PathFindingSystem->SetPathFindingSystem(
		X_Length,
		Y_Length,
		[](UPathFindingSystem* pfs, FGrid grid)
		{
			UPathNode* pathNode = NewObject<UPathNode>();
			pathNode->SetGrid(grid);
			return pathNode;
		}
	);

	//Grid 맵에 장애물 적용. 통과 불가.
	auto map = PathFindingSystem->GetPathNodeMap();
	for (int32 i = 0; i < X_Length; i++)
	{
		for (int32 j = 0; j < Y_Length; j++)
		{
			FGrid grid(i, j);
			FVector pos = GridToWorld(grid);

			FVector startPos = pos + FVector(0.0f, 0.0f, 5000.0f);
			FVector endPos = FVector(startPos.X, startPos.Y, -5000.0f);

			TArray<TEnumAsByte<EObjectTypeQuery>> objects;

			objects.Add(UEngineTypes::ConvertToObjectType(
				ECollisionChannel::ECC_GameTraceChannel2)); // ObjectType : Obstacle Obj Type을 감지함. DefaultEngine.ini 참고

			TArray<AActor*> ignores;
			TArray<FHitResult> outHits;

			UKismetSystemLibrary::LineTraceMultiForObjects(
				GetWorld(),
				startPos,
				endPos,
				objects,
				true,
				ignores,
				EDrawDebugTrace::None,
				//EDrawDebugTrace::ForDuration,
				outHits,
				true,
				FLinearColor::Red,
				FLinearColor::Blue,
				5.0f
			);

			//UE_LOG(LogTemp, Warning, TEXT(" Grid(%d : %d) Trace Num : %d"), i, j, outHits.Num());

			//outhits이 1개 이상의 값을 가지고 있다면, 해당 위치에 장애물이 존재하고 있는 것임.
			if (outHits.Num() > 0)
			{
				UPathNode* pathNode = map[grid];
				if (IsValid(pathNode))
				{
					pathNode->SetIsWalkable(false);
				}
			}

		}
	}
}

TArray<FGrid> ASRPG_GameMode::FindPath(const FGrid& Start, const FGrid& End, int32& PathLength, bool bCanIgnoreUnit, bool bCalculateToTarget)
{
	//!주의! return 하기 전에 PathLength를 변경시켜야함.

		//PathLength의 존재 이유는, 장애물이나 다른 기타 이유로 빙 돌아서 가야할 때
		//이동력에 의해 닿을 수 있는 위치인지 확인해야함.
		//예시) 이동력이 5인 유닛은 해당 위치까지 가는 PathLength가 5 이하일 때만 해당 위치를 갈 수 있음.

		//openList = 이동 가능할 위치. PriorityQueue처럼 사용할 것으로 TArray 
		//closeSet = 이동 불가능함이 확정된 위치. 사용한 것인지만 판독하기 위한 TSet
	TArray<UPathNode*> openList;
	TSet<UPathNode*> closeSet;

	//Heap화 == PriorityQueue
	openList.Heapify(UPathNode::PathFindingPredicated);

	//시작 위치
	UPathNode* startNode = PathFindingSystem->GetValidPathNode(Start);
	if (!IsValid(startNode))
	{
		UE_LOG(LogTemp, Warning, TEXT("StartNode is Not Valid"));
		PathLength = -1;
		return TArray<FGrid>();
	}

	//목표 위치
	UPathNode* endNode = PathFindingSystem->GetValidPathNode(End);
	if (!IsValid(endNode))
	{
		UE_LOG(LogTemp, Warning, TEXT("EndNode is Not Valid"));
		PathLength = -1;
		return TArray<FGrid>();
	}

	//모든 PathNode를 초기화 한 뒤에 시작함.
	InitAllPathFindingNodes();

	startNode->SetGCost(0);
	startNode->SetHCost(CalculateGridDistance(Start, End));
	startNode->CalculateFCost();

	openList.Add(startNode);

	while (openList.Num() > 0)
	{
		UPathNode* currentNode = GetLowestFCostNode(openList);

		if (currentNode == endNode) //Break Point.
		{
			FGrid currentGrid = currentNode->GetGrid();
			AUnitCharacter* currentUnit = GetUnitAtGrid(currentGrid);
			AUnitCharacter* startUnit = GetUnitAtGrid(Start);

			//적군이든 아군이든 누군가가 존재한다면, 해당 Grid를 점유할 수 없으므로
			//경로가 존재할 수 없게됨.
			//다만, 자기 자신이라면 가능함.
			if (!bCalculateToTarget && IsValid(currentUnit) && currentUnit != startUnit)
			{
				PathLength = -1;
				return TArray<FGrid>();
			}

			PathLength = endNode->GetFCost();
			return CalculatePath(endNode);
		}

		openList.Remove(currentNode);
		closeSet.Add(currentNode);

		//UE_LOG(LogTemp, Warning, TEXT("Node Position : %d, %d"), currentNode->GetGrid().X, currentNode->GetGrid().Y);

		TArray<UPathNode*> nearNodeArray = GetNearNodeArray(currentNode);
		for (UPathNode* nearNode : nearNodeArray)
		{
			//close Set 안에 있는 노드는 무시.
			if (closeSet.Contains(nearNode))
			{
				continue;
			}

			//걸을 수 없는 위치 무시.
			if (nearNode->GetIsWalkable() == false)
			{
				closeSet.Add(nearNode);
				continue;
			}

			//bCanIgnoreUnit이 true일 때, 적 유닛을 통과할 수 있음.
			//GridVisual은 여기가 아니라 MoveActionComponent에서 ValidGridVisual을 체크함.
			//유닛 정보는 GridSystem에 접근이 필요함.
			UGridObject* gridObj = GridSystem->GetValidGridObject(nearNode->GetGrid());
			if (!bCanIgnoreUnit && IsValid(gridObj) && gridObj->HasAnyUnit())
			{
				AUnitCharacter* currentUnit = gridObj->GetUnit();
				AUnitCharacter* startUnit = GetUnitAtGrid(Start);
				if (IsValid(currentUnit) && IsValid(startUnit))
				{
					if (currentUnit->ActorHasTag(startUnit->Tags[0]))
					{
						//아군이면 일단 통과가 가능함.
					}
					else
					{
						//아군이 아니라면 통과가 불가능함.
						closeSet.Add(nearNode);
						continue;
					}
				}
			}


			//F = G + H;
			//G : 현재까지의 Cost
			//H : 앞으로 예상되는 Cost
			int tempGCost = currentNode->GetGCost() + CalculateGridDistance(currentNode->GetGrid(), nearNode->GetGrid());

			if (tempGCost < nearNode->GetGCost())
			{
				nearNode->SetParentNode(currentNode);
				nearNode->SetGCost(tempGCost);
				nearNode->SetHCost(CalculateGridDistance(nearNode->GetGrid(), End));
				nearNode->CalculateFCost();

				if (!openList.Contains(nearNode))
				{
					openList.Add(nearNode);
				}
			}
		}
	}

	//openList.Num() > 0인 조건이 끝날 때까지 올바른 경로를 못찾았다면 실패.
	PathLength = -1;
	return TArray<FGrid>();
}

int32 ASRPG_GameMode::CalculateGridDistance(const FGrid& a, const FGrid& b) const
{
	return FMath::Abs(a.X - b.X) + FMath::Abs(a.Y - b.Y);
}

UPathNode* ASRPG_GameMode::GetLowestFCostNode(TArray<UPathNode*>& PathNodeList)
{
	if (PathNodeList.Num() == 0)
	{
		return nullptr;
	}

	//PathNodeList는 FindPath에서 Heapify가 됐다. Heap의 첫번째 원소가 가장 F 값이 낮은 Node다.
	UPathNode* lowestNode = PathNodeList[0];

	return lowestNode;
}

TArray<FGrid> ASRPG_GameMode::CalculatePath(UPathNode* EndNode) const
{
	if (!IsValid(EndNode))
	{
		return TArray<FGrid>();
	}

	//Path를 얻는 법
	//PathFinding을 한 후, 목표 지점 노드부터 Parent를 계속 올라가면서 Grid 값을 얻어낸다.
	//그 Grid 결과를 뒤집으면 Start -> End 까지의 Path다.
	TArray<FGrid> gridArray;

	gridArray.Add(EndNode->GetGrid());
	UPathNode* current = EndNode;
	while (IsValid(current))
	{
		gridArray.Add(current->GetGrid());
		current = current->GetParentNode();
	}

	//결과를 reverse하면 올바른 방향의 경로가 나옴.
	Algo::Reverse(gridArray);

	return gridArray;
}

TArray<UPathNode*> ASRPG_GameMode::GetNearNodeArray(UPathNode* CurrentNode) const
{
	//PathFinding에서 사용할 현재 위치의 상하좌우 Node를 찾는 Function
	//만약, 존재하지 않는 노드라면 NearNode로 추가하지 않음.

	TArray<UPathNode*> nearNodeList;

	FGrid grid = CurrentNode->GetGrid();

	TArray<int32> dx{ 1,-1,0,0 };
	TArray<int32> dy{ 0,0,1,-1 };

	for (int32 i = 0; i < dx.Num(); i++)
	{
		FGrid near = grid;
		near.X += dx[i];
		near.Y += dy[i];

		if (IsValidGrid(near))
		{
			UPathNode* nearNode = PathFindingSystem->GetValidPathNode(near);

			if (!IsValid(nearNode))
			{
				continue;
			}

			nearNodeList.Add(nearNode);
		}
	}

	return nearNodeList;
}

bool ASRPG_GameMode::HasPath(const FGrid& Start, const FGrid& End, bool bCanIgnoreUnit)
{
	//FindPath의 결과 중 PathLength가 -1이면 위치까지의 경로가 존재하지 않음.

	int32 pathLength = 0;
	FindPath(Start, End, pathLength, bCanIgnoreUnit);

	return pathLength != -1;
}

void ASRPG_GameMode::InitAllPathFindingNodes()
{
	//PathFindingGridSystem의 Grid 값을 PathFinding에 이용할 수 있도록 전부 초기화.
	//G = 무한대(int32 최대값) , H = 0 , F = G + H = 무한대

	TMap<FGrid, UPathNode*> pathNodes = PathFindingSystem->GetPathNodeMap();

	for (TPair<FGrid, UPathNode*> pathNodePair : pathNodes)
	{
		if (!IsValid(pathNodePair.Value))
		{
			//UE_LOG(LogTemp, Warning, TEXT("PathNode is Not Valid"));
			continue;
		}

		UPathNode* pathNode = pathNodePair.Value;

		pathNode->SetGCost(TNumericLimits<int32>::Max());
		pathNode->SetHCost(0);
		pathNode->CalculateFCost();
		pathNode->SetParentNode(nullptr);
	}
}

TArray<AUnitCharacter*> ASRPG_GameMode::GetUnitArrayAtGrid(const FGrid& GridValue) const
{
	//Grid 위에 있는 Unit을 얻음.
	//이 Function은 Unit이 여러개일 때를 가정함.

	UGridObject* gridObject = GridSystem->GetValidGridObject(GridValue);
	if (!IsValid(gridObject))
	{
		return TArray<AUnitCharacter*>();
	}


	return gridObject->GetUnitArray();
}

TArray<AUnitCharacter*> ASRPG_GameMode::GetAllUnitInGridSystem() const
{
	//현재 Grid 위에 존재하는 모든 Unit Character를 얻어냄.

	TMap<FGrid, UGridObject*> gridObjs = GetAllGridObjectsThatHasUnit();

	TArray<AUnitCharacter*> unitArray;

	for (auto gridObj : gridObjs)
	{
		auto unit = gridObj.Value->GetUnit();
		if (!IsValid(unit))
		{
			continue;
		}
		unitArray.Add(unit);
	}

	return unitArray;
}

AUnitCharacter* ASRPG_GameMode::GetUnitAtGrid(const FGrid& GridValue) const
{
	//Grid 위에 있는 Unit을 얻음.
		//이 Function은 Unit이 하나일 때를 가정함.

	TArray<AUnitCharacter*> gridArray = GetUnitArrayAtGrid(GridValue);

	if (gridArray.Num() == 0)
	{
		return nullptr;
	}

	return gridArray[0];
}

bool ASRPG_GameMode::HasAnyUnitOnGrid(const FGrid& GridValue) const
{
	//Grid 위에 Unit이 존재하는지 확인.

	UGridObject* gridObj = GridSystem->GetValidGridObject(GridValue);
	if (IsValid(gridObj))
	{
		return gridObj->HasAnyUnit();
	}

	return false;
}

bool ASRPG_GameMode::IsWalkableGrid(const FGrid& GridValue) const
{
	//장애물, 높은 벽, 용암? 등의 유닛이 올라갈 수 없는 위치인지 확인함
	//TODO : 공중 유닛의 경우를 생각해야함.

	UPathNode* pathNode = PathFindingSystem->GetValidPathNode(GridValue);
	if (IsValid(pathNode))
	{
		return pathNode->GetIsWalkable();
	}

	return false;
}

int32 ASRPG_GameMode::GetPathLength(const FGrid& Start, const FGrid& End)
{
	int32 pathLength = 0;
	FindPath(Start, End, pathLength);
	return pathLength;
}

bool ASRPG_GameMode::IsValidGrid(const FGrid& Grid) const
{
	return (Grid.X >= 0) && (Grid.Y >= 0) && (Grid.X < X_Length) && (Grid.Y < Y_Length);
}

FGrid ASRPG_GameMode::WorldToGrid(const FVector& WorldPosition) const
{
	FVector managerLocation;

	if (IsValid(GridManagerRef))
	{
		managerLocation = GridManagerRef->GetActorLocation();
	}

	FGrid grid;

	grid.X = FMath::RoundToInt32((WorldPosition.X - managerLocation.X) / CellSize);
	grid.Y = FMath::RoundToInt32((WorldPosition.Y - managerLocation.Y) / CellSize);

	UE_LOG(LogTemp, Warning, TEXT("Grid Pos : %s"), *grid.ToString());

	return grid;
}

FVector ASRPG_GameMode::GridToWorld(const FGrid& Grid) const
{
	//UE_LOG(LogTemp, Warning, TEXT("Grid Input : %s"), *Grid.ToString());

	FVector managerLocation;

	if (IsValid(GridManagerRef))
	{
		managerLocation = GridManagerRef->GetActorLocation();
	}

	FVector worldPosition = managerLocation;
	worldPosition.X += Grid.X * CellSize;
	worldPosition.Y += Grid.Y * CellSize;

	//UE_LOG(LogTemp, Warning, TEXT("World Pos : %s"),*worldPosition.ToString());

	return worldPosition;
}

UGridObject* ASRPG_GameMode::GetValidGridObject(const FGrid& Grid) const
{
	UGridObject* gridObj = GridSystem->GetValidGridObject(Grid);

	if (!IsValid(gridObj))
	{
		return nullptr;
	}

	return gridObj;
}

void ASRPG_GameMode::AddUnitAtGrid(AUnitCharacter* Unit, const FGrid& GridValue)
{
	if (!IsValid(GridSystem))
	{
		return;
	}

	UGridObject* gridObject = GridSystem->GetValidGridObject(GridValue);
	if (IsValid(gridObject))
	{
		gridObject->AddUnit(Unit);
	}
}

void ASRPG_GameMode::RemoveUnitAtGrid(AUnitCharacter* Unit, const FGrid& GridValue)
{
	if (!IsValid(GridSystem))
	{
		return;
	}

	UGridObject* gridObject = GridSystem->GetValidGridObject(GridValue);
	if (IsValid(gridObject))
	{
		gridObject->RemoveUnit(Unit);
	}
}

void ASRPG_GameMode::MoveUnitGrid(AUnitCharacter* Unit, const FGrid& From, const FGrid& to)
{
	RemoveUnitAtGrid(Unit, From);
	AddUnitAtGrid(Unit, to);

	//Unit이 자신의 현재 위치를 보유하고 있음.
	Unit->SetGrid(to);

	if (OnAnyUnitMoved.IsBound())
	{
		OnAnyUnitMoved.Broadcast();
	}
}

TMap<FGrid, UGridObject*> ASRPG_GameMode::GetAllGridObjectsThatHasUnit() const
{
	if (!IsValid(GridSystem))
	{
		return TMap<FGrid, UGridObject*>();
	}

	return GridSystem->GetAllGridObjectsThatHasUnit();
}

void ASRPG_GameMode::InitGridSetting()
{
	AGridManager* gridManager = Cast<AGridManager>(
	UGameplayStatics::GetActorOfClass(GetWorld(), AGridManager::StaticClass()));

	if (IsValid(gridManager))
	{
		gridManager->Setup();
	}
}

void ASRPG_GameMode::SetupGridManaging(AGridManager* GridManager)
{
	if (IsValid(GridManager))
	{
		GridManagerRef = GridManager;
	}

	X_Length = GridManagerRef->X_Length;
	Y_Length = GridManagerRef->Y_Length;
	CellSize = GridManagerRef->CellSize;

	CreateGridSystem();

	TArray<AActor*> units;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AUnitCharacter::StaticClass(), units);
	for (auto actor : units)
	{
		AUnitCharacter* unit = Cast<AUnitCharacter>(actor);
		if (IsValid(unit))
		{
			unit->InitUnit();
		}
	}

	TArray<AActor*> controlPawns;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AUnitControlPawn::StaticClass(), controlPawns);

	for (auto actor : controlPawns)
	{
		AUnitControlPawn* controlpawn = Cast<AUnitControlPawn>(actor);

		if (IsValid(controlpawn))
		{
			controlpawn->InitGridPosition(controlpawn->StartGrid);
		}
	}

}
