// Fill out your copyright notice in the Description page of Project Settings.


#include "TurnManager.h"
#include "Kismet/GameplayStatics.h"
#include "UnitCore/UnitCharacter.h"
#include "UnitControl/EnemyUnitControlPawn.h"
#include "UnitControl/UnitControlPawn.h"
// Sets default values
ATurnManager::ATurnManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TurnNumber = 1;
	TurnType = ETurnType::PlayerTurn;
}

// Called when the game starts or when spawned
void ATurnManager::BeginPlay()
{
	Super::BeginPlay();

	//InitTurnManager();

}

void ATurnManager::InitTurnManager()
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
			AddUnitForTurnManager(unit);
		}
	}

	EnemyUnitControlPawnRef = Cast<AEnemyUnitControlPawn>(UGameplayStatics::GetActorOfClass(GetWorld(), AEnemyUnitControlPawn::StaticClass()));

	TArray<AActor*> controlPawnArr;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AUnitControlPawn::StaticClass(), controlPawnArr);
	for (auto controlpawn : controlPawnArr)
	{
		auto controlPawn_Casted= Cast<AUnitControlPawn>(controlpawn);
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

// Called every frame
void ATurnManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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

void ATurnManager::NextTurn()
{
	switch(TurnType)
	{
		case ETurnType::PlayerTurn:
			SetTurnType(ETurnType::EnemyTurn);
			break;
		case ETurnType::EnemyTurn:
			SetTurnType(ETurnType::AllyTurn);
			break;
		case ETurnType::AllyTurn:
			SetTurnType(ETurnType::PlayerTurn);
			NextTurnNumber();
			break;
	}
}

void ATurnManager::ResetTurn()
{
	TurnNumber = 1;
	TurnType = ETurnType::PlayerTurn;
}

void ATurnManager::CheckCurrentTurnValidation()
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



void ATurnManager::SetTurnType(ETurnType TurnTypeInput)
{
	TurnType = TurnTypeInput;

	switch (TurnType)
	{
	case ETurnType::PlayerTurn:
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

void ATurnManager::NextTurnNumber()
{
	TurnNumber++;

	if (OnPlayerTurnStart.IsBound())
	{
		OnPlayerTurnStart.Broadcast();
	}

}

void ATurnManager::StartGame()
{
	SetTurnType(ETurnType::PlayerTurn);
	TurnNumber = 1;
}

void ATurnManager::AddUnitForTurnManager(AUnitCharacter* UnitToAdd)
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


	FName unitTag = UnitToAdd->Tags[0];

	if (unitTag == FName("MyUnit"))
	{
		OnPlayerTurnStart.AddDynamic(UnitToAdd, &AUnitCharacter::StartUnitTurn);
		PlayerUnitArr.Add(UnitToAdd);
	}
	else if (unitTag == FName("Enemy"))
	{
		OnEnemyTurnStart.AddDynamic(UnitToAdd, &AUnitCharacter::StartUnitTurn);
		EnemyUnitArr.Add(UnitToAdd);
	}

	UnitToAdd->OnFinishAllAction.AddDynamic(this, &ATurnManager::CheckCurrentTurnValidation);

}

void ATurnManager::RemoveUnitFromTurnManager(AUnitCharacter* UnitToRemove)
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

	UnitToRemove->OnFinishAllAction.RemoveDynamic(this, &ATurnManager::CheckCurrentTurnValidation);

}

