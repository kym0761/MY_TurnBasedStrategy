// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Turn/Turn.h"
#include "TurnManager.generated.h"


class AUnitCharacter;
class AEnemyUnitControlPawn;
class AUnitControlPawn;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTurnManagerDelegate);


UCLASS()
class TURNBASEDSTRATEGY_API ATurnManager : public AActor
{
	GENERATED_BODY()
	
private:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Turn", Meta = (AllowPrivateAccess = true))
		int32 TurnNumber = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Turn", Meta = (AllowPrivateAccess = true))
		ETurnType TurnType;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Turn", Meta = (AllowPrivateAccess = true))
		TArray<AUnitCharacter*> PlayerUnitArr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Turn", Meta = (AllowPrivateAccess = true))
		TArray<AUnitCharacter*> EnemyUnitArr;

	UPROPERTY()
		AEnemyUnitControlPawn* EnemyUnitControlPawnRef;

	UPROPERTY()
		AUnitControlPawn* PlayerUnitControlPawnRef;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (BitMask, BitmaskEnum = "EBitMaskExample"))
		int32 BitmaskExample;

public:	
	// Sets default values for this actor's properties
	ATurnManager();

	/*턴 시작했을 때 각 진영의 유닛들에게 Callback.*/
	FTurnManagerDelegate OnPlayerTurnStart;
	FTurnManagerDelegate OnEnemyTurnStart;
	FTurnManagerDelegate OnAllyTurnStart; //!! 아직 Ally를 안만듬.

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	static ATurnManager* GetTurnManager();

	//Temp.. For Call in test UI 
	UFUNCTION(BlueprintCallable)
		void InitTurnManager();

	//temp...to Act Turn Correctly
	UFUNCTION(BlueprintCallable, CallInEditor)
		void NextTurn();
	UFUNCTION(BlueprintCallable, CallInEditor)
		void ResetTurn();
	UFUNCTION()
		void CheckCurrentTurnValidation();

	void SetTurnType(ETurnType TurnTypeInput);

	void NextTurnNumber();
	void StartGame();

	void AddUnitForTurnManager(AUnitCharacter* UnitToAdd);
	void RemoveUnitFromTurnManager(AUnitCharacter* UnitToRemove);
};
