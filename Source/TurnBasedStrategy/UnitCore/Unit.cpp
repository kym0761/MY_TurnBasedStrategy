// Fill out your copyright notice in the Description page of Project Settings.


#include "Unit.h"

#include "UnitAction/UnitMoveActionComponent.h"
#include "UnitAction/UnitAttackActionComponent.h"
#include "UnitAction/UnitInteractActionComponent.h"
#include "UnitAction/WaitActionComponent.h"
#include "StatComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Components/SceneComponent.h"

#include "Manager/GridManager.h"
#include "Manager/SRPG_GameMode.h"
#include "UMG/DamageTextActor.h"

#include "DebugHelper.h"

// Sets default values
AUnit::AUnit()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	StatComponent = CreateDefaultSubobject<UStatComponent>("StatComponent");

	UnitMoveActionComponent = CreateDefaultSubobject<UUnitMoveActionComponent>("UnitMoveActionComponent");
	UnitAttackActionComponent = CreateDefaultSubobject<UUnitAttackActionComponent>("UnitAttackActionComponent");
	UnitInteractActionComponent = CreateDefaultSubobject<UUnitInteractActionComponent>("UnitInteractActionComponent");
	WaitActionComponent = CreateDefaultSubobject<UWaitActionComponent>("WaitActionComponent");

}

// Called when the game starts or when spawned
void AUnit::BeginPlay()
{
	Super::BeginPlay();
	
}

void AUnit::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ASRPG_GameMode* gameMode = ASRPG_GameMode::GetSRPG_GameMode(GetWorld());
	if (IsValid(gameMode))
	{
		gameMode->RemoveUnitAtGrid(this, gameMode->WorldToGrid(GetActorLocation()));
		gameMode->RemoveUnitFromTurnManaging(this);
	}

	Super::EndPlay(EndPlayReason);

}

// Called every frame
void AUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

float AUnit::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (!IsValid(StatComponent))
	{
		return 0.0f;
	}

	StatComponent->DealDamage(DamageAmount);

	if (IsValid(DamageTextActorBP))
	{
		ADamageTextActor* damageTextActor = GetWorld()->SpawnActorDeferred<ADamageTextActor>(DamageTextActorBP, GetActorTransform());
		if (IsValid(damageTextActor))
		{
			damageTextActor->AppliedDamage = DamageAmount;
			damageTextActor->FinishSpawning(GetActorTransform());
		}
	}

	if (StatComponent->GetHP() <= 0.0f)
	{
		SetLifeSpan(2.5f);
	}

	return DamageAmount;
}

FGrid AUnit::GetGrid()
{
	ASRPG_GameMode* gameMode = ASRPG_GameMode::GetSRPG_GameMode(GetWorld());
	if (IsValid(gameMode))
	{
		return gameMode->WorldToGrid(GetActorLocation());
	}

	return FGrid(-1, -1);
}

bool AUnit::HasActionComponent(EUnitActionType UnitActionType)
{
	switch (UnitActionType)
	{
	case EUnitActionType::Move:
		return IsValid(UnitMoveActionComponent);

	case EUnitActionType::Attack:
		return IsValid(UnitAttackActionComponent);

	case EUnitActionType::Interact:
		return IsValid(UnitInteractActionComponent);

	default:
		return false;
	}
}

UUnitActionComponent* AUnit::GetUnitActionComponent(EUnitActionType UnitActionType)
{
	switch (UnitActionType)
	{
	case EUnitActionType::Move:
		return UnitMoveActionComponent;

	case EUnitActionType::Attack:
		return UnitAttackActionComponent;

	case EUnitActionType::Interact:
		return UnitInteractActionComponent;

	default:
		return nullptr;
	}
}

void AUnit::InitUnit()
{
	ASRPG_GameMode* gameMode = ASRPG_GameMode::GetSRPG_GameMode(GetWorld());
	if (IsValid(gameMode))
	{

		gameMode->AddUnitAtGrid(this, gameMode->WorldToGrid(GetActorLocation()));
	}
}

void AUnit::StartUnitTurn()
{
	ActivateUnitAllAction();
}

void AUnit::OnSelectedUnitChanged()
{
	Debug::Print(DEBUG_TEXT("OnSelectedUnitChanged -> ") + *GetActorLabel());

}

void AUnit::ActivateUnitAllAction()
{
	//���� ���۵��� ��, �� ������ �� �� �ִ� �ൿ�� ���� �� Set���ش�.

	TArray<UActorComponent*> unitActions;
	GetComponents(UUnitActionComponent::StaticClass(), unitActions);

	for (UActorComponent* unitAction : unitActions)
	{
		UUnitActionComponent* unitAction_Cast =
			Cast<UUnitActionComponent>(unitAction);

		if (!IsValid(unitAction_Cast))
		{
			continue;
		}

		unitAction_Cast->SetCanDoActionThisTurn(true);
	}
}

void AUnit::FinishUnitAllAction()
{
	//������ ��� �׼��� �̹��Ͽ� ��Ȱ��ȭ�Ͽ� �ൿ�� ��ģ ������ ��.
	//�̴� ������ ����ģ ��, Ȥ�� Wait �ϸ� ������ ��.

	TArray<UActorComponent*> unitActions;
	GetComponents(UUnitActionComponent::StaticClass(), unitActions);

	for (UActorComponent* unitAction : unitActions)
	{
		UUnitActionComponent* unitAction_Cast =
			Cast<UUnitActionComponent>(unitAction);

		if (!IsValid(unitAction_Cast))
		{
			continue;
		}

		unitAction_Cast->SetCanDoActionThisTurn(false);
	}

	if (OnFinishAllAction.IsBound())
	{
		OnFinishAllAction.Broadcast();
	}
}

bool AUnit::IsThisUnitCanAction() const
{
	//������ �׼��� �����Ѱ�? == �� �׼��� �˻��ϰ� �׼� �߿� ���� �Ͽ� ��� ������ ���� �ִ��� �� Ȯ���ϸ� �ȴ�.

	TArray<UActorComponent*> unitActions;
	GetComponents(UUnitActionComponent::StaticClass(), unitActions);

	for (UActorComponent* unitAction : unitActions)
	{
		UUnitActionComponent* unitAction_Cast =
			Cast<UUnitActionComponent>(unitAction);

		if (!IsValid(unitAction_Cast))
		{
			continue;
		}

		if (unitAction_Cast->IsCanDoActionThisTurn())
		{
			return true;
		}
	}

	return false;
}

