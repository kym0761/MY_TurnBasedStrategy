// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitCharacter.h"
#include "StatComponent.h"

#include "UnitAction/UnitMoveActionComponent.h"
#include "UnitAction/UnitAttackActionComponent.h"
#include "UnitAction/UnitInteractActionComponent.h"
#include "UnitAction/WaitActionComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Manager/GridManager.h"
#include "Manager/SRPG_GameMode.h"
#include "UnitControl/UnitSelectPawn.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "../UMG/DamageTextActor.h"

#include "Engine/DamageEvents.h"

// Sets default values
AUnitCharacter::AUnitCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StatComponent = CreateDefaultSubobject<UStatComponent>("StatComponent");

	UnitMoveActionComponent = CreateDefaultSubobject<UUnitMoveActionComponent>("UnitMoveActionComponent");
	UnitAttackActionComponent = CreateDefaultSubobject<UUnitAttackActionComponent>("UnitAttackActionComponent");
	UnitInteractActionComponent = CreateDefaultSubobject<UUnitInteractActionComponent>("UnitInteractActionComponent");
	WaitActionComponent = CreateDefaultSubobject<UWaitActionComponent>("WaitActionComponent");

	GetCharacterMovement()->SetFixedBrakingDistance(20.0f);
}

// Called when the game starts or when spawned
void AUnitCharacter::BeginPlay()
{
	Super::BeginPlay();

}

void AUnitCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
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
void AUnitCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AUnitCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float AUnitCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (!IsValid(StatComponent))
	{
		return 0.0f;
	}

	StatComponent->DealDamage(DamageAmount);

	if (IsValid(DamageTextActorBP))
	{
		ADamageTextActor* damageTextActor = GetWorld()->SpawnActorDeferred<ADamageTextActor>(DamageTextActorBP,GetActorTransform());
		if (IsValid(damageTextActor))
		{
			damageTextActor->AppliedDamage = DamageAmount;
			damageTextActor->FinishSpawning(GetActorTransform());
		}
	}

	if (StatComponent->GetHP() <= 0.0f)
	{
		SetLifeSpan(5.0f);
	}

	return DamageAmount;
}

FGrid AUnitCharacter::GetGrid()
{
	return Grid;
}

void AUnitCharacter::SetGrid(FGrid GridValue)
{
	Grid = GridValue;
}

UUnitActionComponent* AUnitCharacter::GetUnitActionComponent(EUnitActionType UnitActionType)
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

void AUnitCharacter::InitUnit()
{
	ASRPG_GameMode* gameMode = ASRPG_GameMode::GetSRPG_GameMode(GetWorld());
	if (IsValid(gameMode))
	{
		Grid = gameMode->WorldToGrid(GetActorLocation());
		gameMode->AddUnitAtGrid(this, Grid);
	}
}

void AUnitCharacter::StartUnitTurn()
{
	//Function when TurnChanged.
	//UE_LOG(LogTemp, Warning, TEXT("StartUnitTurn -> %s"), *GetActorLabel());

	ActivateUnitAllAction();
}

bool AUnitCharacter::HasActionComponent(EUnitActionType UnitActionType)
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

void AUnitCharacter::OnSelectedUnitChanged()
{	
	AUnitSelectPawn* unitSelectPawn = AUnitSelectPawn::GetUnitSelectPawn();
	if (IsValid(unitSelectPawn))
	{
		if (unitSelectPawn->GetSelectedUnit() == this)
		{
			UE_LOG(LogTemp, Warning, TEXT("OnSelectedUnitChanged -> %s"), *GetActorLabel());
		}
	}
}

void AUnitCharacter::ActivateUnitAllAction()
{
	//턴이 시작됐을 때, 이 유닛이 할 수 있는 행동을 전부 다 Set해준다.

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

void AUnitCharacter::FinishUnitAllAction()
{
	//유닛의 모든 액션을 이번턴에 비활성화하여 행동을 마친 것으로 함.
	//이는 공격을 끝마친 후, 혹은 Wait 하면 동작할 것.

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

bool AUnitCharacter::IsThisUnitCanAction() const
{
	//유닛은 액션이 가능한가? == 각 액션을 검색하고 액션 중에 현재 턴에 사용 가능한 것이 있는지 다 확인하면 된다.

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

