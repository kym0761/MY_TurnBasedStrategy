// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TurnManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTurnManagerDelegate);

UENUM(BlueprintType)
enum class ETurnType : uint8
{
	PlayerTurn UMETA(DisplayName = "PlayerTurn"),
	EnemyTurn UMETA(DisplayName = "EnemyTurn"),
	AllyTurn UMETA(DisplayName = "AllyTurn")

};


UCLASS()
class TURNBASEDSTRATEGY_API ATurnManager : public AActor
{
	GENERATED_BODY()
	
private:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Turn", Meta = (AllowPrivateAccess = true))
	int32 TurnNumber = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Turn", Meta = (AllowPrivateAccess = true))
		ETurnType TurnType;

public:	
	// Sets default values for this actor's properties
	ATurnManager();

	FTurnManagerDelegate OnTurnChanged;

	FTurnManagerDelegate OnPlayerTurnStart;
	FTurnManagerDelegate OnEnemyTurnStart;
	FTurnManagerDelegate OnAllyTurnStart;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	static ATurnManager* GetTurnManager();

	//temp...to Act Turn Correctly
	UFUNCTION(BlueprintCallable, CallInEditor)
		void NextTurn();
	UFUNCTION(BlueprintCallable, CallInEditor)
		void ResetTurn();

	void SetTurnType(ETurnType TurnTypeInput);

	void NextTurnNumber();

	void StartGame();

};
