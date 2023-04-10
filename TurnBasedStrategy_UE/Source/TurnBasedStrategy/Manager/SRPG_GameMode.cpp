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
	// ��� ������ ã��, ������ Player�� ��������, �ƴϸ� Enemy���� Ȯ���ؼ� �з���.
	// �������� UnitControlPawn���Ե� �ش� ������ ���� ���ε��� ���̶�� ���� �˷���.

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
		////!! Ally�� ����� �Ǹ� �� �κ��� ������ ��.
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
		//!!��� �ʿ�.
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
	//Attacker�� ������ �¾Ҵٸ� Defender�� hit �ִϸ��̼� �����.

	FAttackOrder currentOrder = OrderToPlay[0];

	if (!IsValid(currentOrder.Defender) || !IsValid(currentOrder.Attacker))
	{
		return;
	}

	//ũ��Ƽ�� ����
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
	//������ ��� ó���ϱ�?

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
	// ���� ������ 1 ������ 2 �����
	// ���� �ӵ� ���̿� ���� �����ڿ� ����ڰ� �߰� ������ ���� ���� ����.
	// ���� ��ų ���� ������ ���� �����ڿ� ������� �ʱ� ������ ���� Ƚ���� �ٲ� ���� ����.

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

	//Attacker�� ���ǵ尡 5���� �� ������ ������ �߰���.
	if (attackerSpeed > defenderSpeed + 5)
	{
		attackOrders.Add(attack);
	}

	//Defender�� ���ǵ尡 5���� �� ������ ������ �߰���.
	if (attackerSpeed +5 < defenderSpeed)
	{
		attackOrders.Add(counterAttack);
	}

	//Attacker�� ���ǵ尡 10���� �� ������ �߰��� 1/2�� ��������ŭ ������ �߰���.
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

	//defender�� ���ǵ尡 10���� �� ������ �߰��� 1/2�� ��������ŭ ������ �߰���.
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
	//critRate 0.0f~1.0f ���� ��.

	UStatComponent* attackerStatComponent =
		Attacker->FindComponentByClass<UStatComponent>();

	UStatComponent* defenderStatComponent =
		Defender->FindComponentByClass<UStatComponent>();

	if (!(IsValid(attackerStatComponent) && IsValid(defenderStatComponent)))
	{
		//�Ұ����� ũ��Ƽ�� Ȯ����.
		return -1.0f;
	}

	int32 attackerSKL = attackerStatComponent->GetSKL();
	int32 attackerLUK = attackerStatComponent->GetLUK();

	int32 defenderSKL = defenderStatComponent->GetSKL();
	int32 defenderLUK = defenderStatComponent->GetLUK();

	//ũ�� ���� = (������ Skill - ����� Skill) + (������ �� - ����� ��) / 100.0 + ������ SKill + ������ ��
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
	//���߷��� 0~1 ���� ����.
	//TODO : ���߷��� �����ֱ⸸ �Ǵ� ���̰� ����� ���� ����� ���Ⱑ�� ����.
	//			���ݿ� ���߷��� �����ؼ� ȸ�ǵ� �ǰ� ��������.


	UStatComponent* attackerStatComponent =
		Attacker->FindComponentByClass<UStatComponent>();

	UStatComponent* defenderStatComponent =
		Defender->FindComponentByClass<UStatComponent>();

	if (!(IsValid(attackerStatComponent) && IsValid(defenderStatComponent)))
	{
		//�Ұ����� ���߷���.
		return -1.0f;
	}

	int32 attackerSpeed = attackerStatComponent->GetSPD();
	int32 attackerSKL = attackerStatComponent->GetSKL();
	int32 attackerLUK = attackerStatComponent->GetLUK();

	int32 defenderSpeed = defenderStatComponent->GetSPD();
	int32 defenderSKL = defenderStatComponent->GetSKL();
	int32 defenderLUK = defenderStatComponent->GetLUK();


	//���߷� ������ + ��ų ���� + ���ǵ� ���� + �������� ��� / 100
	//�������� ���� ���̵��� ���� ����� ���� ���� ��?
	float correction = 80.0f;
	float accuracy = (correction + (attackerSKL - defenderSKL) + (attackerSpeed - defenderSpeed) + attackerLUK) / 100.0f;

	//���߷��� 0~1���̷� Clamp
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

		//������ �׾����� ���� Attack ���� ����.
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

	//Grid �ʿ� ��ֹ� ����. ��� �Ұ�.
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
				ECollisionChannel::ECC_GameTraceChannel2)); // ObjectType : Obstacle Obj Type�� ������. DefaultEngine.ini ����

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

			//outhits�� 1�� �̻��� ���� ������ �ִٸ�, �ش� ��ġ�� ��ֹ��� �����ϰ� �ִ� ����.
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
	//!����! return �ϱ� ���� PathLength�� ������Ѿ���.

		//PathLength�� ���� ������, ��ֹ��̳� �ٸ� ��Ÿ ������ �� ���Ƽ� ������ ��
		//�̵��¿� ���� ���� �� �ִ� ��ġ���� Ȯ���ؾ���.
		//����) �̵����� 5�� ������ �ش� ��ġ���� ���� PathLength�� 5 ������ ���� �ش� ��ġ�� �� �� ����.

		//openList = �̵� ������ ��ġ. PriorityQueueó�� ����� ������ TArray 
		//closeSet = �̵� �Ұ������� Ȯ���� ��ġ. ����� �������� �ǵ��ϱ� ���� TSet
	TArray<UPathNode*> openList;
	TSet<UPathNode*> closeSet;

	//Heapȭ == PriorityQueue
	openList.Heapify(UPathNode::PathFindingPredicated);

	//���� ��ġ
	UPathNode* startNode = PathFindingSystem->GetValidPathNode(Start);
	if (!IsValid(startNode))
	{
		UE_LOG(LogTemp, Warning, TEXT("StartNode is Not Valid"));
		PathLength = -1;
		return TArray<FGrid>();
	}

	//��ǥ ��ġ
	UPathNode* endNode = PathFindingSystem->GetValidPathNode(End);
	if (!IsValid(endNode))
	{
		UE_LOG(LogTemp, Warning, TEXT("EndNode is Not Valid"));
		PathLength = -1;
		return TArray<FGrid>();
	}

	//��� PathNode�� �ʱ�ȭ �� �ڿ� ������.
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

			//�����̵� �Ʊ��̵� �������� �����Ѵٸ�, �ش� Grid�� ������ �� �����Ƿ�
			//��ΰ� ������ �� ���Ե�.
			//�ٸ�, �ڱ� �ڽ��̶�� ������.
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
			//close Set �ȿ� �ִ� ���� ����.
			if (closeSet.Contains(nearNode))
			{
				continue;
			}

			//���� �� ���� ��ġ ����.
			if (nearNode->GetIsWalkable() == false)
			{
				closeSet.Add(nearNode);
				continue;
			}

			//bCanIgnoreUnit�� true�� ��, �� ������ ����� �� ����.
			//GridVisual�� ���Ⱑ �ƴ϶� MoveActionComponent���� ValidGridVisual�� üũ��.
			//���� ������ GridSystem�� ������ �ʿ���.
			UGridObject* gridObj = GridSystem->GetValidGridObject(nearNode->GetGrid());
			if (!bCanIgnoreUnit && IsValid(gridObj) && gridObj->HasAnyUnit())
			{
				AUnitCharacter* currentUnit = gridObj->GetUnit();
				AUnitCharacter* startUnit = GetUnitAtGrid(Start);
				if (IsValid(currentUnit) && IsValid(startUnit))
				{
					if (currentUnit->ActorHasTag(startUnit->Tags[0]))
					{
						//�Ʊ��̸� �ϴ� ����� ������.
					}
					else
					{
						//�Ʊ��� �ƴ϶�� ����� �Ұ�����.
						closeSet.Add(nearNode);
						continue;
					}
				}
			}


			//F = G + H;
			//G : ��������� Cost
			//H : ������ ����Ǵ� Cost
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

	//openList.Num() > 0�� ������ ���� ������ �ùٸ� ��θ� ��ã�Ҵٸ� ����.
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

	//PathNodeList�� FindPath���� Heapify�� �ƴ�. Heap�� ù��° ���Ұ� ���� F ���� ���� Node��.
	UPathNode* lowestNode = PathNodeList[0];

	return lowestNode;
}

TArray<FGrid> ASRPG_GameMode::CalculatePath(UPathNode* EndNode) const
{
	if (!IsValid(EndNode))
	{
		return TArray<FGrid>();
	}

	//Path�� ��� ��
	//PathFinding�� �� ��, ��ǥ ���� ������ Parent�� ��� �ö󰡸鼭 Grid ���� ����.
	//�� Grid ����� �������� Start -> End ������ Path��.
	TArray<FGrid> gridArray;

	gridArray.Add(EndNode->GetGrid());
	UPathNode* current = EndNode;
	while (IsValid(current))
	{
		gridArray.Add(current->GetGrid());
		current = current->GetParentNode();
	}

	//����� reverse�ϸ� �ùٸ� ������ ��ΰ� ����.
	Algo::Reverse(gridArray);

	return gridArray;
}

TArray<UPathNode*> ASRPG_GameMode::GetNearNodeArray(UPathNode* CurrentNode) const
{
	//PathFinding���� ����� ���� ��ġ�� �����¿� Node�� ã�� Function
	//����, �������� �ʴ� ����� NearNode�� �߰����� ����.

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
	//FindPath�� ��� �� PathLength�� -1�̸� ��ġ������ ��ΰ� �������� ����.

	int32 pathLength = 0;
	FindPath(Start, End, pathLength, bCanIgnoreUnit);

	return pathLength != -1;
}

void ASRPG_GameMode::InitAllPathFindingNodes()
{
	//PathFindingGridSystem�� Grid ���� PathFinding�� �̿��� �� �ֵ��� ���� �ʱ�ȭ.
	//G = ���Ѵ�(int32 �ִ밪) , H = 0 , F = G + H = ���Ѵ�

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
	//Grid ���� �ִ� Unit�� ����.
	//�� Function�� Unit�� �������� ���� ������.

	UGridObject* gridObject = GridSystem->GetValidGridObject(GridValue);
	if (!IsValid(gridObject))
	{
		return TArray<AUnitCharacter*>();
	}


	return gridObject->GetUnitArray();
}

TArray<AUnitCharacter*> ASRPG_GameMode::GetAllUnitInGridSystem() const
{
	//���� Grid ���� �����ϴ� ��� Unit Character�� ��.

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
	//Grid ���� �ִ� Unit�� ����.
		//�� Function�� Unit�� �ϳ��� ���� ������.

	TArray<AUnitCharacter*> gridArray = GetUnitArrayAtGrid(GridValue);

	if (gridArray.Num() == 0)
	{
		return nullptr;
	}

	return gridArray[0];
}

bool ASRPG_GameMode::HasAnyUnitOnGrid(const FGrid& GridValue) const
{
	//Grid ���� Unit�� �����ϴ��� Ȯ��.

	UGridObject* gridObj = GridSystem->GetValidGridObject(GridValue);
	if (IsValid(gridObj))
	{
		return gridObj->HasAnyUnit();
	}

	return false;
}

bool ASRPG_GameMode::IsWalkableGrid(const FGrid& GridValue) const
{
	//��ֹ�, ���� ��, ���? ���� ������ �ö� �� ���� ��ġ���� Ȯ����
	//TODO : ���� ������ ��츦 �����ؾ���.

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

	//Unit�� �ڽ��� ���� ��ġ�� �����ϰ� ����.
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
