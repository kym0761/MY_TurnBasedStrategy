// Fill out your copyright notice in the Description page of Project Settings.


#include "TurnManager.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ATurnManager::ATurnManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TurnNumber = 1;
	Turn = ETurnType::PlayerTurn;
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
	switch(Turn)
	{
		case ETurnType::PlayerTurn:
			Turn = ETurnType::EnemyTurn;
			break;
		case ETurnType::EnemyTurn:
			Turn = ETurnType::AllyTurn;
			break;
		case ETurnType::AllyTurn:
			Turn = ETurnType::PlayerTurn;
			NextTurnNumber();
			break;
	}
}

void ATurnManager::ResetTurn()
{
	TurnNumber = 1;
	Turn = ETurnType::PlayerTurn;
}

void ATurnManager::NextTurnNumber()
{
	TurnNumber++;

	if (OnTurnChanged.IsBound())
	{
		OnTurnChanged.Broadcast();
	}

}

