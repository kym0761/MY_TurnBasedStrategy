// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnitControl/UnitControlPawn.h"
#include "EnemyUnitControlPawn.generated.h"

/**
 * 
 */
UCLASS(abstract)
class TURNBASEDSTRATEGY_API AEnemyUnitControlPawn : public AUnitControlPawn
{
	GENERATED_BODY()

public:
	
	AEnemyUnitControlPawn();

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Unit", Meta = (AllowPrivateAccess = true))
	TArray<AUnitCharacter*> EnemyUnits;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void TriggerToPlay();

	void FindEnemyAllUnits();

	void MoveProcedure();

	virtual void OnUnitActionCompleted() override;
};
