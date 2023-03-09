// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnitControl/UnitControlPawn.h"
#include "EnemyUnitControlPawn.generated.h"

/**
 * 
 */
UCLASS()
class TURNBASEDSTRATEGY_API AEnemyUnitControlPawn : public AUnitControlPawn
{
	GENERATED_BODY()

public:
	
	AEnemyUnitControlPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	TArray<AUnitCharacter*> EnemyUnits;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void FindEnemyAllUnits();

	void MoveProcedure();

	void OnUnitMoveFinished();
};
