// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnitControl/UnitControlPawn.h"
#include "EnemyUnitControlPawn.generated.h"

class AUnit;

UENUM(BlueprintType)
enum class EAIPawnMode : uint8
{
	NONE UMETA(DisplayName = "NONE"),

	Move UMETA(DisplayName = "Move"),
	Attack UMETA(DisplayName = "Attack"),
	Wait UMETA(DisplayName = "Wait")

};

/**
 * Enemy를 움직이게 할 AI ControlPawn.
 */
UCLASS(abstract)
class TURNBASEDSTRATEGY_API AEnemyUnitControlPawn : public AUnitControlPawn
{
	GENERATED_BODY()

public:

	AEnemyUnitControlPawn();

private:

	//UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Unit", Meta = (AllowPrivateAccess = true))
	//	TArray<AUnit*> EnemyUnits;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Unit", Meta = (AllowPrivateAccess = true))
	EAIPawnMode AIPawnMode = EAIPawnMode::NONE;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void TriggerToPlay();

	void MoveProcedure();
	void AttackProcedure();
	void WaitProcedure();

	void DoAIControl();

	virtual void OnUnitActionCompleted_virtual() override;
};
