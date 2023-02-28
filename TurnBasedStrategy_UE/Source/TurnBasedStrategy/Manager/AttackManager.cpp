// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackManager.h"
#include "Kismet/GameplayStatics.h"
#include "UnitCore/UnitAnimInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "UnitAction/UnitAttackActionComponent.h"
#include "UnitCore/StatComponent.h"
#include "UnitCore/UnitCharacter.h"
#include "GridManager.h"

// Sets default values
AAttackManager::AAttackManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAttackManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAttackManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAttackManager::SetupAttackManager(AActor* Attacker, AActor* Defender)
{
	//Attacker = InAttacker;
	//Defender = InDefender;

	if (!IsValid(Attacker) || !IsValid(Defender))
	{
		return;
	}

	OrderToPlay.Empty();
	OrderToPlay = CalculateAttackOrder(Attacker,Defender);

}

void AAttackManager::StartAttack()
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

void AAttackManager::PlayAttack()
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

void AAttackManager::FinishAttack()
{
	if (IsValid(CurrentAttackActionComponent))
	{
		CurrentAttackActionComponent->TakeAction(FGrid(0,0));
	}

	//reset
	bAttackerWaiting = false;
	bDefenderWaiting = false;
	CurrentAttackOrder = FAttackOrder();

}

void AAttackManager::OnAttackHit()
{
	//Attacker의 공격을 맞았다면 Defender의 hit 애니메이션 재생함.

	FAttackOrder& currentOrder = OrderToPlay[0];
	AActor* currentDefender = currentOrder.Defender;

	if (!IsValid(currentOrder.Defender) || !IsValid(currentOrder.Attacker))
	{
		return;
	}

	UGameplayStatics::ApplyDamage(currentOrder.Defender, currentOrder.Damage, nullptr, currentOrder.Attacker, UDamageType::StaticClass());

	if (!IsValid(currentDefender))
	{
		return;
	}

	USkeletalMeshComponent* defenderMesh = currentOrder.Defender->FindComponentByClass<USkeletalMeshComponent>();
	if (!IsValid(defenderMesh))
	{
		return;
	}

	UUnitAnimInstance* defenderAnim = Cast<UUnitAnimInstance>(defenderMesh->GetAnimInstance());
	if (IsValid(defenderAnim))
	{
		//UE_LOG(LogTemp, Warning, TEXT("Play Hit Montage?"));
		defenderAnim->PlayUnitHitMontage();
	}

}

void AAttackManager::OnAttackEnd()
{
	bAttackerWaiting = true;
	UE_LOG(LogTemp, Warning, TEXT("AAttackManager::OnAttackEnd()"));
	TryPlayNextOrder();
}

void AAttackManager::OnHit()
{
	//아마 데미지 받은 량이 나오는 Actor를 Spawn?

	//FAttackOrder& currentOrder = OrderToPlay[0];
	//if (!IsValid(currentOrder.Defender) || !IsValid(currentOrder.Attacker))
	//{
	//	return;
	//}

	//UGameplayStatics::ApplyDamage(currentOrder.Defender, currentOrder.Damage, nullptr, currentOrder.Attacker,UDamageType::StaticClass());

}

void AAttackManager::OnHitEnd()
{
	bDefenderWaiting = true;
	UE_LOG(LogTemp, Warning, TEXT("AAttackManager::OnHitEnd()"));
	TryPlayNextOrder();
}

TArray<FAttackOrder> AAttackManager::CalculateAttackOrder(AActor* Attacker, AActor* Defender)
{
	//현재 공격 Order는 임의로 공격자 우선 -> 방어자 반격임.

	TArray<FAttackOrder> attackOrders;

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

	FAttackOrder attack;
	attack.AttackOrderType = EAttackOrderType::Attack;
	int32 attackerDamage = attackerStatComponent->GetSTR();
	attack.Damage = attackerDamage;
	attack.Attacker = Attacker;
	attack.Defender = Defender;

	FAttackOrder counterAttack;
	counterAttack.AttackOrderType = EAttackOrderType::Defend;
	int32 defenderDamage = defenderStatComponent->GetSTR();
	counterAttack.Damage = defenderDamage;
	counterAttack.Attacker = Defender;
	counterAttack.Defender = Attacker;

	// 테스트 용도로 공/반/공/공 형식으로 공격함.
	// 추후 SPD 값 혹은	보유 스킬에 따라 공격 횟수가 달라질 수 있음.
	attackOrders.Add(attack);
	attackOrders.Add(counterAttack);
	attackOrders.Add(attack);
	attackOrders.Add(attack);


	return 	attackOrders;
}

AAttackManager* AAttackManager::GetAttackManager()
{
	if (!IsValid(GEngine) || !IsValid(GEngine->GameViewport))
	{
		return nullptr;
	}

	UWorld* world = GEngine->GameViewport->GetWorld();
	AAttackManager* attackManager = Cast<AAttackManager>(UGameplayStatics::GetActorOfClass(world, AAttackManager::StaticClass()));

	return attackManager;
}

void AAttackManager::BindOnAttackHit(UUnitAnimInstance* AnimInst)
{
	if (!IsValid(AnimInst))
	{
		return;
	}

	FScriptDelegate toBind;
	toBind.BindUFunction(this, FName("OnAttackHit"));

	AnimInst->BindTo_OnAttackHit(toBind);

}

void AAttackManager::BindOnAttackEnd(UUnitAnimInstance* AnimInst)
{
	if (!IsValid(AnimInst))
	{
		return;
	}

	FScriptDelegate toBind;
	toBind.BindUFunction(this, FName("OnAttackEnd"));

	AnimInst->BindTo_OnAttackEnd(toBind);
}

void AAttackManager::BindOnHit(UUnitAnimInstance* AnimInst)
{
	if (!IsValid(AnimInst))
	{
		return;
	}

	FScriptDelegate toBind;
	toBind.BindUFunction(this, FName("OnHit"));

	AnimInst->BindTo_OnHit(toBind);
}

void AAttackManager::BindOnHitEnd(UUnitAnimInstance* AnimInst)
{
	if (!IsValid(AnimInst))
	{
		return;
	}

	FScriptDelegate toBind;
	toBind.BindUFunction(this, FName("OnHitEnd"));

	AnimInst->BindTo_OnHitEnd(toBind);
}

void AAttackManager::TryPlayNextOrder()
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

TArray<FAttackOrder> AAttackManager::GetAttackOrder() const
{
	return OrderToPlay;
}

int32 AAttackManager::CalculateGridValue_ToAttack(AActor* Attacker, AActor* Defender)
{
	//Unit : 팀 분간 필요.
    //Grid : Test할 위치.

	if (!IsValid(Attacker) || !IsValid(Defender))
	{
		return -1;
	}

	TArray<FAttackOrder> attackOrders = CalculateAttackOrder(Attacker, Defender);

	UStatComponent* attackerStatComponent =
		Attacker->FindComponentByClass<UStatComponent>();

	UStatComponent* defenderStatComponent =
		Defender->FindComponentByClass<UStatComponent>();

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

	float attackValue = 100* (defenderStatComponent->GetMaxHP() - defenderHP) / defenderStatComponent->GetMaxHP();

	valueScore += FMath::Clamp<int32>(attackValue, 0.0f, 100.0f);

	return (int32)valueScore;
}

FAttackOrder::FAttackOrder()
{
	AttackOrderType = EAttackOrderType::Attack;
	Damage = 0;
	Attacker = nullptr;
	Defender = nullptr;
}
