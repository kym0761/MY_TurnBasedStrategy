// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UnitManager.generated.h"

class AUnitCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAnyUnitSpawned);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAnyUnitDead);

UCLASS()
class TURNBASEDSTRATEGY_API AUnitManager : public AActor
{
	GENERATED_BODY()
	
private:
	 TArray<AUnitCharacter*> UnitArray;
	 TArray<AUnitCharacter*> FriendlyUnitArray;
	 TArray<AUnitCharacter*> EnemyUnitArray;

public:	
	// Sets default values for this actor's properties
	AUnitManager();

	FOnAnyUnitSpawned OnAnyUnitSpawned;
	FOnAnyUnitDead OnAnyUnitDead;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	static AUnitManager* GetUnitManager();

	TArray<AUnitCharacter*> GetUnitArray() const;
	TArray<AUnitCharacter*> GetFriendlyUnitArray() const;
	TArray<AUnitCharacter*> GetEnemyUnitArray() const;

};
