// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackManager.h"
#include "../StatComponent.h"
#include "Kismet/GameplayStatics.h"
#include "../UnitAnimInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "../UnitAction/UnitAttackActionComponent.h"
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

void AAttackManager::SetupAttackManager(AActor* InAttacker, AActor* InDefender)
{
	Attacker = InAttacker;
	Defender = InDefender;
}

void AAttackManager::StartAttack()
{
	if (IsValid(Attacker))
	{
		FRotator attackerLookRot = UKismetMathLibrary::FindLookAtRotation(Attacker->GetActorLocation(), Defender->GetActorLocation());
		Attacker->SetActorRotation(attackerLookRot);

	}

	if (IsValid(Defender))
	{
		FRotator defenderLookRot = UKismetMathLibrary::FindLookAtRotation(Defender->GetActorLocation(), Attacker->GetActorLocation());
		Defender->SetActorRotation(defenderLookRot);
	}
	
	CurrentAttacker = nullptr;
	CurrentDefender = nullptr;

	OrderToPlay.Empty();

	OrderToPlay = CalculateAttackOrder();

	PlayAttack();
}

void AAttackManager::PlayAttack()
{

	if (OrderToPlay.Num() == 0)
	{
		FinishAttack();
		return;
	}

	FAttackOrder& currentOrder = OrderToPlay[0];

	CurrentAttacker = nullptr;
	CurrentDefender = nullptr;

	switch (currentOrder.AttackOrderType)
	{
	case EAttackOrderType::Attack:
		CurrentAttacker = Attacker;
		CurrentDefender = Defender;
		break;
	case EAttackOrderType::Defend:
		CurrentAttacker = Defender;
		CurrentDefender = Attacker;
		break;
	}

	if (IsValid(CurrentAttacker) && IsValid(CurrentDefender))
	{
		USkeletalMeshComponent* attackerMesh = CurrentAttacker->FindComponentByClass<USkeletalMeshComponent>();
		USkeletalMeshComponent* defenderMesh = CurrentDefender->FindComponentByClass<USkeletalMeshComponent>();

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
			
				//UE_LOG(LogTemp, Warning, TEXT("Play Montage?"));
				attackerAnim->PlayUnitAttackMontage();
				bAttackerWaiting = false;
				bDefenderWaiting = false;
			}
		}
	}

	if (OrderToPlay.Num() > 0)
	{
		OrderToPlay.RemoveAt(0);
	}
}

void AAttackManager::FinishAttack()
{
	UUnitAttackActionComponent* attackComponent = Attacker->FindComponentByClass<UUnitAttackActionComponent>();

	if (IsValid(attackComponent))
	{
		attackComponent->TakeAction(FGrid(0,0));
	}


	//reset
	Attacker = nullptr;
	Defender = nullptr;
	CurrentAttacker = nullptr;
	CurrentDefender = nullptr;
	bAttackerWaiting = false;
	bDefenderWaiting = false;
}

void AAttackManager::OnAttackHit()
{
	if (!IsValid(CurrentDefender))
	{
		return;
	}

	USkeletalMeshComponent* defenderMesh = CurrentDefender->FindComponentByClass<USkeletalMeshComponent>();
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
	TryPlayNextOrder();
}

void AAttackManager::OnHit()
{
	//아마 데미지 받은 량이 나오는 Actor를 Spawn?
}

void AAttackManager::OnHitEnd()
{
	bDefenderWaiting = true;
	TryPlayNextOrder();
}

TArray<FAttackOrder> AAttackManager::CalculateAttackOrder()
{
	TArray<FAttackOrder> attackOrders;


	if (!IsValid(Attacker) || !IsValid(Defender))
	{
		UE_LOG(LogTemp, Warning, TEXT("Attacker or Defender is Not Valid"));
		return attackOrders;
	}

	UStatComponent* attackerStatComponent =
		Attacker->FindComponentByClass<UStatComponent>();

	UStatComponent* defenderStatComponent =
		Defender->FindComponentByClass<UStatComponent>();

	if (!IsValid(attackerStatComponent) || !IsValid(defenderStatComponent))
	{
		UE_LOG(LogTemp, Warning, TEXT("StatComponents are Not Valid"));

		return attackOrders;
	}

	FAttackOrder attack;
	attack.AttackOrderType = EAttackOrderType::Attack;
	int32 attackerDamage = attackerStatComponent->GetSTR();
	attack.Damage = attackerDamage;

	FAttackOrder defend;
	defend.AttackOrderType = EAttackOrderType::Defend;
	int32 defenerDamage = defenderStatComponent->GetSTR();
	defend.Damage = defenerDamage;

	attackOrders.Add(attack);
	attackOrders.Add(defend);

	return attackOrders;
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

	AnimInst->BindToOnAttackHit(toBind);

}

void AAttackManager::BindOnAttackEnd(UUnitAnimInstance* AnimInst)
{
	if (!IsValid(AnimInst))
	{
		return;
	}

	FScriptDelegate toBind;
	toBind.BindUFunction(this, FName("OnAttackEnd"));

	AnimInst->BindToOnAttackEnd(toBind);
}

void AAttackManager::BindOnHit(UUnitAnimInstance* AnimInst)
{
	if (!IsValid(AnimInst))
	{
		return;
	}

	FScriptDelegate toBind;
	toBind.BindUFunction(this, FName("OnHit"));

	AnimInst->BindToOnHit(toBind);
}

void AAttackManager::BindOnHitEnd(UUnitAnimInstance* AnimInst)
{
	if (!IsValid(AnimInst))
	{
		return;
	}

	FScriptDelegate toBind;
	toBind.BindUFunction(this, FName("OnHitEnd"));

	AnimInst->BindToHitEnd(toBind);
}

void AAttackManager::TryPlayNextOrder()
{
	if (bAttackerWaiting == true && bDefenderWaiting == true)
	{
		PlayAttack();
	}
}

