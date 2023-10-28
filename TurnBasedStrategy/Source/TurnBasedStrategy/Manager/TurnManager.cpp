// Fill out your copyright notice in the Description page of Project Settings.


#include "TurnManager.h"
#include "Kismet/GameplayStatics.h"
#include "UnitControl/UnitControlPawn.h"
#include "UnitCore/Unit.h"

#include "DebugHelper.h"

// Sets default values
ATurnManager::ATurnManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

ATurnManager* ATurnManager::GetTurnManager()
{
	if (!IsValid(GEngine) || !IsValid(GEngine->GameViewport))
	{
		return nullptr;
	}

	UWorld* world = GEngine->GameViewport->GetWorld();

	ATurnManager* turnManager = Cast<ATurnManager>(UGameplayStatics::GetActorOfClass(world, ATurnManager::StaticClass()));

	return turnManager;
}

// Called when the game starts or when spawned
void ATurnManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATurnManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATurnManager::InitTurn()
{

	TArray<AActor*> controlPawnArr;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AUnitControlPawn::StaticClass(), controlPawnArr);
	for (auto controlpawn : controlPawnArr)
	{
		if (!IsValid(controlpawn))
		{
			continue;
		}

		IControlPawnInterface* cpi = Cast<IControlPawnInterface>(controlpawn);
		if (cpi)
		{
			cpi->FindAllManagingUnits();
		}
	}

}

void ATurnManager::NextTurn()
{
	Debug::Print(DEBUG_TEXT("Next Turn"));

	//if (!bIsBusy)
	//{
	//	Debug::Print(DEBUG_TEXT("currently Busy"));
	//	return;
	//}

	switch (TurnType)
	{
	case ETurnType::Team01Turn:
		SetTurnType(ETurnType::Team02Turn);
		break;
	case ETurnType::Team02Turn:
		//SetTurnType(ETurnType::AllyTurn);
		////!! Ally를 만들게 되면 이 부분을 변경할 것.
		NextTurnNumber();
		SetTurnType(ETurnType::Team01Turn);
		break;
		//case ETurnType::AllyTurn:
		//	SetTurnType(ETurnType::PlayerTurn);
		//	NextTurnNumber();
		//	break;
	default:
		break;
	}
}

void ATurnManager::ResetTurn()
{
	TurnNumber = 1;
	TurnType = ETurnType::Team01Turn;
}

bool ATurnManager::CheckCurrentTurnValidation()
{
	//각 유닛이 행동이 가능한지 가능하지 않은지를 확인해서
	//가능한 유닛이 있으면 턴을 true
	//행동 가능한 유닛이 없으면 false한다.

	bool IsTurnValid = false;

	TArray<AActor*> outUnits;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AUnit::StaticClass(), outUnits);

	switch (TurnType)
	{
	case ETurnType::Team01Turn:

		for (auto i : outUnits)
		{
			auto unit = CastChecked<AUnit>(i);

			if (unit->GetTeamType() != ETeamType::Team01)
			{
				continue;
			}

			if (unit->IsThisUnitCanAction())
			{
				IsTurnValid = true;
				break;
			}
		}
		break;

	case ETurnType::Team02Turn:

		for (auto i : outUnits)
		{
			auto unit = CastChecked<AUnit>(i);

			if (unit->GetTeamType() != ETeamType::Team02)
			{
				continue;
			}

			if (unit->IsThisUnitCanAction())
			{
				IsTurnValid = true;
				break;
			}
		}
		break;

	case ETurnType::Team03Turn:
		break;
	default:
		break;
	}

	return IsTurnValid;

	/*if (IsTurnValid == false)
	{
		Debug::Print(DEBUG_TEXT("Turn Will be Changed"));
		NextTurn();
	}*/
}

void ATurnManager::SetTurnType(ETurnType TurnTypeInput)
{
	TurnType = TurnTypeInput;

	switch (TurnType)
	{
	case ETurnType::Team01Turn:
		if (OnPlayerTurnStart.IsBound())
		{
			OnPlayerTurnStart.Broadcast();
		}
		break;
	case ETurnType::Team02Turn:
		if (OnEnemyTurnStart.IsBound())
		{
			OnEnemyTurnStart.Broadcast();
		}

		break;
	case ETurnType::Team03Turn:
		break;
	default:
		break;
	}

	if (OnTurnChanged.IsBound())
	{
		OnTurnChanged.Broadcast();
	}
	

}

void ATurnManager::NextTurnNumber()
{
	TurnNumber++;

	// 유닛들 행동 전부 회복시켜야함.
	TArray<AActor*> outUnits;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AUnit::StaticClass(), outUnits);

	for (auto i : outUnits)
	{
		auto unit = CastChecked<AUnit>(i);

		unit->ActivateUnitAllActions();
	}

}

void ATurnManager::StartTurn()
{
	ResetTurn();
}

void ATurnManager::SetIsBusy(bool BusyVal)
{
	bIsBusy = BusyVal;

	if (!bIsBusy && OnIsBusyChanged.IsBound())
	{
		OnIsBusyChanged.Broadcast();
	}

}

bool ATurnManager::IsBusy() const
{
	return bIsBusy;
}

ETurnType ATurnManager::GetTurnType() const
{
	return TurnType;
}
