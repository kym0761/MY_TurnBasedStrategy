// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleManager.h"
#include "DebugHelper.h"

#include "UnitCore/Unit.h"
#include "UnitCore/StatComponent.h"
#include "UnitCore/UnitAnimInstance.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "UnitAction/UnitAttackActionComponent.h"
#include "GridManager.h"

// Sets default values
ABattleManager::ABattleManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

ABattleManager* ABattleManager::GetBattleManager()
{
	if (!IsValid(GEngine) || !IsValid(GEngine->GameViewport))
	{
		return nullptr;
	}

	UWorld* world = GEngine->GameViewport->GetWorld();
	ABattleManager* battleManager = Cast<ABattleManager>(UGameplayStatics::GetActorOfClass(world, ABattleManager::StaticClass()));

	return battleManager;
}

// Called when the game starts or when spawned
void ABattleManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABattleManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

TArray<FBattleOrder> ABattleManager::CalculateBattleOrders(AActor* Attacker, AActor* Defender)
{
	// 공격 순서는 1 공격자 2 방어자
	// 공격 속도 차이에 따라 공격자와 방어자가 추가 공격을 행할 수가 있음.
	// 추후 스킬 존재 유무에 따라 공격자와 방어자의 초기 순서와 공격 횟수가 바뀔 수도 있음.

	TArray<FBattleOrder> orders;

	if (!IsValid(Attacker) || !IsValid(Defender))
	{
		Debug::Print(DEBUG_TEXT("Attacker or Defender is Invalid."));
		return orders;
	}

	UStatComponent* attackerStatComponent =
		Attacker->FindComponentByClass<UStatComponent>();
	UStatComponent* defenderStatComponent =
		Defender->FindComponentByClass<UStatComponent>();

	if (!IsValid(attackerStatComponent) || !IsValid(defenderStatComponent))
	{
		Debug::Print(DEBUG_TEXT("Attacker or Defender's StatComponents are Invalid."));
		return orders;
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

	//공격자의 공격
	FBattleOrder attack;
	attack.OrderOwnerType = EOrderOwnerType::Attacker;
	attack.Damage = attackerDamage - defenderDEF;
	attack.Attacker = Attacker;
	attack.Defender = Defender;
	attack.Accuracy = CalculateAccuracy(Attacker, Defender);
	attack.CritRate = CalculateCriticalRate(Attacker, Defender);

	//방어자의 공격
	FBattleOrder counterAttack;
	counterAttack.OrderOwnerType = EOrderOwnerType::Defender;
	counterAttack.Damage = defenderDamage - defenderDEF;
	counterAttack.Attacker = Defender;
	counterAttack.Defender = Attacker;
	counterAttack.Accuracy = CalculateAccuracy(Defender, Attacker);
	counterAttack.CritRate = CalculateCriticalRate(Defender, Attacker);

	//일단, 둘다 공격이 보장됨.
	orders.Add(attack);
	orders.Add(counterAttack);

	//Attacker의 스피드가 5보다 더 높으면 공격을 추가함.
	if (attackerSpeed > defenderSpeed + 5)
	{
		orders.Add(attack);
	}

	//Defender의 스피드가 5보다 더 높으면 공격을 추가함.
	if (attackerSpeed + 5 < defenderSpeed)
	{
		orders.Add(counterAttack);
	}

	//Attacker의 스피드가 10보다 더 높으면 추가로 1/2의 데미지만큼 공격을 추가함.
	if (attackerSpeed > defenderSpeed + 10)
	{
		FBattleOrder addtionalAttack;
		addtionalAttack.OrderOwnerType = EOrderOwnerType::Attacker;
		addtionalAttack.Damage = attack.Damage / 2;
		addtionalAttack.Attacker = Attacker;
		addtionalAttack.Defender = Defender;
		addtionalAttack.Accuracy = attack.Accuracy;
		addtionalAttack.CritRate = attack.CritRate;
		orders.Add(addtionalAttack);
	}

	//defender의 스피드가 10보다 더 높으면 추가로 1/2의 데미지만큼 공격을 추가함.
	if (attackerSpeed + 10 < defenderSpeed)
	{
		FBattleOrder addtionalAttack;
		addtionalAttack.OrderOwnerType = EOrderOwnerType::Defender;
		addtionalAttack.Damage = counterAttack.Damage / 2;
		addtionalAttack.Attacker = Defender;
		addtionalAttack.Defender = Attacker;
		addtionalAttack.Accuracy = counterAttack.Accuracy;
		addtionalAttack.CritRate = counterAttack.CritRate;
		orders.Add(addtionalAttack);
	}

	return orders;
}

float ABattleManager::CalculateCriticalRate(AActor* Attacker, AActor* Defender)
{
	//critRate 0.0f~1.0f 사이 값.

	UStatComponent* attackerStatComponent =
		Attacker->FindComponentByClass<UStatComponent>();

	UStatComponent* defenderStatComponent =
		Defender->FindComponentByClass<UStatComponent>();

	if (!(IsValid(attackerStatComponent) && IsValid(defenderStatComponent)))
	{
		//불가능한 크리티컬 확률임.
		Debug::Print(DEBUG_TEXT("Impossible Crit Rate."));
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

float ABattleManager::CalculateAccuracy(AActor* Attacker, AActor* Defender)
{
	//명중률은 0~1 사이 값임. 0.0f == 0%, 0.5f == 50%, 1.0f = 100% 

	UStatComponent* attackerStatComponent =
		Attacker->FindComponentByClass<UStatComponent>();

	UStatComponent* defenderStatComponent =
		Defender->FindComponentByClass<UStatComponent>();

	if (!(IsValid(attackerStatComponent) && IsValid(defenderStatComponent)))
	{
		//불가능한 명중률임.
		Debug::Print(DEBUG_TEXT("Impossible Accuracy."));
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

void ABattleManager::SetupBattle(AActor* Attacker, AActor* Defender)
{
	if (!IsValid(Attacker) || !IsValid(Defender))
	{
		Debug::Print(DEBUG_TEXT("Battle Actors are Not Set Correctly."));
		return;
	}

	//먼저 Battle Manager의 공격을 전부 Clear한다.
	//그 후에 전투할 유닛 둘에 대한 Battle Orders를 만든다.
	OrdersToPlay.Empty();
	OrdersToPlay = CalculateBattleOrders(Attacker, Defender);
}

void ABattleManager::StartBattle(AActor* Attacker, AActor* Defender)
{
	SetupBattle(Attacker, Defender);

	AttackActionUnit = Attacker;

	if (OrdersToPlay.IsEmpty())
	{
		Debug::Print(DEBUG_TEXT("Can't Start Battle. Because Order is Invalid."));
		return;
	}

	if (!IsValid(Attacker) || !IsValid(Defender))
	{
		Debug::Print(DEBUG_TEXT("Battle Actors are Not Set Correctly."));
		return;
	}


	//공격자와 방어자 둘이 서로 바라보게 만든다.
	FRotator attackerLookRot = UKismetMathLibrary::FindLookAtRotation(Attacker->GetActorLocation(), Defender->GetActorLocation());
	Attacker->SetActorRotation(attackerLookRot);

	FRotator defenderLookRot = UKismetMathLibrary::FindLookAtRotation(Defender->GetActorLocation(), Attacker->GetActorLocation());
	Defender->SetActorRotation(defenderLookRot);
	
	//Start 세팅이 끝나면 Play
	PlayBattle();
}

void ABattleManager::PlayBattle()
{
	if (OrdersToPlay.IsEmpty())
	{
		//공격을 끝내야함.
		TODO_Marker::TODO();
		return;
	}

	FBattleOrder currentOrder = OrdersToPlay[0];

	AActor* attacker = currentOrder.Attacker;
	AActor* defender = currentOrder.Defender;

	if (!IsValid(attacker) || !IsValid(defender))
	{
		Debug::Print(DEBUG_TEXT("Battle Actors are Invalid."));
		return;
	}

	USkeletalMeshComponent* attackerMesh = attacker->FindComponentByClass<USkeletalMeshComponent>();
	USkeletalMeshComponent* defenderMesh = defender->FindComponentByClass<USkeletalMeshComponent>();

	if (!IsValid(attackerMesh) || !IsValid(defenderMesh))
	{
		Debug::Print(DEBUG_TEXT(" Actor Meshes are Invalid."));
		return;
	}

	UUnitAnimInstance* attackerAnim = Cast<UUnitAnimInstance>(attackerMesh->GetAnimInstance());
	UUnitAnimInstance* defenderAnim = Cast<UUnitAnimInstance>(defenderMesh->GetAnimInstance());

	if (!IsValid(attackerAnim) || !IsValid(defenderAnim))
	{
		Debug::Print(DEBUG_TEXT(" Actor Anims are Invalid."));
		return;
	}

	//BindOnAttackHit(attackerAnim);
	//BindOnAttackEnd(attackerAnim);
	//BindOnHit(defenderAnim);
	//BindOnHitEnd(defenderAnim);

	bAttackerWaiting = false;
	bDefenderWaiting = false;

	attackerAnim->PlayUnitAttackMontage();


}

void ABattleManager::OnPlayBattleAnimationCompleted()
{

	//아직 Attacker , Defender가 animation 재생을 완료하지 못했으면 진입 불가능.
	if (!bAttackerWaiting || !bDefenderWaiting)
	{
		return;
	}

	if (!OrdersToPlay.IsEmpty())
	{
		OrdersToPlay.RemoveAt(0);
	}
	
	if (!OrdersToPlay.IsEmpty())
	{
		FBattleOrder& currentOrder = OrdersToPlay[0];

		if (IsValid(currentOrder.Defender) && IsValid(currentOrder.Attacker))
		{
			UStatComponent* defenderStat = currentOrder.Defender->FindComponentByClass<UStatComponent>();
			UStatComponent* attackerStat = currentOrder.Attacker->FindComponentByClass<UStatComponent>();

			if (IsValid(defenderStat) && IsValid(attackerStat))
			{
				if (defenderStat->GetHP() <= 0.0f || attackerStat->GetHP() <= 0.0f)
				{
					OrdersToPlay.Empty(); //죽은 캐릭터가 존재한다면 Order를 수행할 수 없기 때문에 전부 비워버린다.
				}
			}
		}
	}

	if (!OrdersToPlay.IsEmpty())
	{
		PlayBattle();
	}
	else
	{
		FinishBattle();
	}

}

void ABattleManager::FinishBattle()
{
	//만약 AttackActionUnit이 Set되어 있지 않는다면 발동되지 않을 것이다.
	//예시) 어딘가에서 유닛이 죽었을 때 Battle과 관련없을 상황이면 AttackUnit이 Set되지 않았을 것이므로..
	if (!IsValid(AttackActionUnit))
	{
		Debug::Print(DEBUG_TEXT("AttackActionUnit Invalid."));
		return;
	}

	//배틀을 시도한 유닛의 AttackAction에 TakeAction 처리를 해주어야함.
	UUnitActionComponent* unitAttackActionComponent =
		AttackActionUnit->FindComponentByClass<UUnitAttackActionComponent>();

	if (IsValid(unitAttackActionComponent))
	{
		AGridManager* gridManager = AGridManager::GetGridManager();
		FGrid unitGrid = gridManager->WorldToGrid(AttackActionUnit->GetActorLocation());
		unitAttackActionComponent->TakeAction(unitGrid);
	}

	//reset
	bAttackerWaiting = false;
	bDefenderWaiting = false;
	AttackActionUnit = nullptr;

	Debug::Print(DEBUG_TEXT("Finish Battle OK"));
}

void ABattleManager::OnAttackHit()
{
	//Attacker의 공격을 맞았다면 Defender의 hit 애니메이션 재생함.
	//TODO : 빗나감 처리 필요.

	FBattleOrder currentOrder = OrdersToPlay[0];

	if (!IsValid(currentOrder.Defender) || !IsValid(currentOrder.Attacker))
	{
		Debug::Print(DEBUG_TEXT("Attacker or Defender are Invalid."));
		return;
	}

	USkeletalMeshComponent* defenderMesh = currentOrder.Defender->FindComponentByClass<USkeletalMeshComponent>();
	if (!IsValid(defenderMesh))
	{
		Debug::Print(DEBUG_TEXT("Defender Mesh is Invalid."));
		return;
	}

	UUnitAnimInstance* defenderAnim = Cast<UUnitAnimInstance>(defenderMesh->GetAnimInstance());
	if (!IsValid(defenderAnim))
	{
		Debug::Print(DEBUG_TEXT("Defender Anim is Invalid."));
		return;
	}


	//크리티컬 예시 : 뎀지 2배로 증가
	float randCrit = FMath::FRandRange(0.0f, 1.0f);
	if (currentOrder.CritRate >= randCrit)
	{
		currentOrder.Damage *= 2;
	}

	float randAccuracy = FMath::FRandRange(0.0f, 1.0f);
	if (currentOrder.Accuracy >= randAccuracy)
	{
		UGameplayStatics::ApplyDamage(currentOrder.Defender, currentOrder.Damage, nullptr, currentOrder.Attacker, UDamageType::StaticClass());
		defenderAnim->PlayUnitHitMontage();
	}
	else
	{
		//빗나간 경우 처리하기?
		//빗나감! 텍스트 표시라던지..
		TODO_Marker::TODO();
		defenderAnim->PlayUnitAvoidMontage();
	}

}

void ABattleManager::OnAttackEnd()
{
	Debug::Print(DEBUG_TEXT("OnAttackEnd"));
	bAttackerWaiting = true;
	OnPlayBattleAnimationCompleted();
}

void ABattleManager::OnHit()
{
	Debug::Print(DEBUG_TEXT("OnHit"));
	//사실 이거 필요한지 모르겠음.
}

void ABattleManager::OnHitEnd()
{
	Debug::Print(DEBUG_TEXT("OnHitEnd"));
	bDefenderWaiting = true;
	OnPlayBattleAnimationCompleted();
}

void ABattleManager::OnUnitDestroyed()
{
	//죽은 캐릭터는 OnHitEnd가 Call되지 않는다.
	//Battle이 끝났을테니 FinishBattle()을 Call하자.
	Debug::Print(DEBUG_TEXT("OnUnitDestroyed"));
	FinishBattle();

}

TArray<FBattleOrder> ABattleManager::GetBattleOrder() const
{
	return OrdersToPlay;
}




