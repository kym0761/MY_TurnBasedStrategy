// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitManager.h"
#include "Kismet/GameplayStatics.h"
#include "UnitCore/UnitCharacter.h"
// Sets default values
AUnitManager::AUnitManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AUnitManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AUnitManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

AUnitManager* AUnitManager::GetUnitManager()
{
	if (!IsValid(GEngine) || !IsValid(GEngine->GameViewport))
	{
		return nullptr;
	}

	UWorld* world = GEngine->GameViewport->GetWorld();
	AUnitManager* unitManager = Cast<AUnitManager>(UGameplayStatics::GetActorOfClass(world, AUnitManager::StaticClass()));

	return unitManager;
}

TArray<AUnitCharacter*> AUnitManager::GetUnitArray() const
{
	return UnitArray;
}

TArray<AUnitCharacter*> AUnitManager::GetFriendlyUnitArray() const
{
	return FriendlyUnitArray;
}

TArray<AUnitCharacter*> AUnitManager::GetEnemyUnitArray() const
{
	return EnemyUnitArray;
}


