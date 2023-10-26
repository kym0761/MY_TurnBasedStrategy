// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Turn.h"
#include "TurnManager.generated.h"

class AUnit;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTurnManagingDelegate);

UCLASS()
class TURNBASEDSTRATEGY_API ATurnManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATurnManager();

	static ATurnManager* GetTurnManager();

	FTurnManagingDelegate OnPlayerTurnStart;
	FTurnManagingDelegate OnEnemyTurnStart;

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Turn", Meta = (AllowPrivateAccess = true))
	bool bIsBusy;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Turn", Meta = (AllowPrivateAccess = true))
	int32 TurnNumber = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Turn", Meta = (AllowPrivateAccess = true))
	ETurnType TurnType;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Turn", Meta = (AllowPrivateAccess = true))
	TArray<AUnit*> PlayerUnitArr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Turn", Meta = (AllowPrivateAccess = true))
	TArray<AUnit*> EnemyUnitArr;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//test UI에서 사용하기 위해 일단 UFUNCTION으로 했다. 
	UFUNCTION(BlueprintCallable)
	void InitTurn();

	UFUNCTION(BlueprintCallable, CallInEditor)
	void NextTurn();
	UFUNCTION(BlueprintCallable, CallInEditor)
	void ResetTurn();

	UFUNCTION()
	void CheckCurrentTurnValidation();

	void SetTurnType(ETurnType TurnTypeInput);

	void NextTurnNumber();
	void StartTurn();

	void SetIsBusy(bool BusyVal);
	bool IsBusy() const;

};
