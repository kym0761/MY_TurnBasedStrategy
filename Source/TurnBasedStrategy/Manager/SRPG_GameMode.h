// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Turn.h"
#include "Grid/Grid.h"
#include "GameFramework/GameModeBase.h"
#include "SRPG_GameMode.generated.h"

//class AUnitCharacter;
class AUnit;
class AEnemyUnitControlPawn;
class AUnitControlPawn;

class UUnitAnimInstance;
class UUnitAttackActionComponent;

class UGridObject;
class UPathNode;
class AUnitCharacter;
class UBillboardComponent;
class UInstancedGridVisualComponent;
class UPathFindingSystem;
class UGridSystem;
class AGridManager;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTurnManagingDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGridManagingDelegate);

/**
 * 
 */
UCLASS()
class TURNBASEDSTRATEGY_API ASRPG_GameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:

	ASRPG_GameMode();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

public:

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	static ASRPG_GameMode* GetSRPG_GameMode(const UObject* WorldContextObject);

#pragma region GridManaging

public:
	FGridManagingDelegate OnAnyUnitMoved;
private:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Grid", Meta = (AllowPrivateAccess = true))
		UGridSystem* GridSystem;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Grid", Meta = (AllowPrivateAccess = true))
		UPathFindingSystem* PathFindingSystem;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Grid", Meta = (AllowPrivateAccess = true))
		AGridManager* GridManagerRef;

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = true))
		int32 X_Length;
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = true))
		int32 Y_Length;
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = true))
		float CellSize;

protected:
	//Init
	void CreateGridSystem();

public:

	/*PathFinding Functions*/
	TArray<FGrid> FindPath(const FGrid& Start, const FGrid& End, int32& PathLength, bool bCanIgnoreUnit = false, bool bCalculateToTarget = false);
	int32 CalculateGridDistance(const FGrid& a, const FGrid& b) const;
	UPathNode* GetLowestFCostNode(TArray<UPathNode*>& PathNodeList);
	TArray<FGrid> CalculatePath(UPathNode* EndNode) const;
	TArray<UPathNode*> GetNearNodeArray(UPathNode* CurrentNode) const;
	bool HasPath(const FGrid& Start, const FGrid& End, bool bCanIgnoreUnit = false);
	void InitAllPathFindingNodes();

	/*Grid System Functions*/
	//TArray<AUnitCharacter*> GetUnitArrayAtGrid(const FGrid& GridValue) const;
	//TArray<AUnitCharacter*> GetAllUnitInGridSystem() const;
	//AUnitCharacter* GetUnitAtGrid(const FGrid& GridValue) const;
	TArray<AUnit*> GetUnitArrayAtGrid(const FGrid& GridValue) const;
	TArray<AUnit*> GetAllUnitInGridSystem() const;
	AUnit* GetUnitAtGrid(const FGrid& GridValue) const;
	bool HasAnyUnitOnGrid(const FGrid& GridValue) const;
	bool IsWalkableGrid(const FGrid& GridValue) const;
	int32 GetPathLength(const FGrid& Start, const FGrid& End);
	bool IsValidGrid(const FGrid& Grid) const;
	FGrid WorldToGrid(const FVector& WorldPosition) const;
	FVector GridToWorld(const FGrid& Grid) const;
	UGridObject* GetValidGridObject(const FGrid& Grid) const;
	void AddUnitAtGrid(AUnit* Unit, const FGrid& GridValue);
	void RemoveUnitAtGrid(AUnit* Unit, const FGrid& GridValue);
	void MoveUnitGrid(AUnit* Unit, const FGrid& From, const FGrid& to);
	TMap<FGrid, UGridObject*> GetAllGridObjectsThatHasUnit() const;

	//TestUI에서 Call됨.
	UFUNCTION(BlueprintCallable)
	void InitGridSetting();
	void SetupGridManaging(AGridManager* GridManager);

#pragma endregion

#pragma region TurnManaging

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Turn", Meta = (AllowPrivateAccess = true))
		int32 TurnNumber = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Turn", Meta = (AllowPrivateAccess = true))
		ETurnType TurnType;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Turn", Meta = (AllowPrivateAccess = true))
		TArray<AUnit*> PlayerUnitArr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Turn", Meta = (AllowPrivateAccess = true))
		TArray<AUnit*> EnemyUnitArr;

	UPROPERTY()
		AEnemyUnitControlPawn* EnemyUnitControlPawnRef;

	UPROPERTY()
		AUnitControlPawn* PlayerUnitControlPawnRef;

public:

	/*턴 시작했을 때 각 진영의 유닛들에게 Callback.*/
	FTurnManagingDelegate OnPlayerTurnStart;
	FTurnManagingDelegate OnEnemyTurnStart;
	//FTurnManagingDelegate OnAllyTurnStart; //!! 아직 Ally를 안만듬.

	//Temp.. For Call in test UI 
	UFUNCTION(BlueprintCallable)
		void InitTurn();

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

	void AddUnitForTurnManaging(AUnit* UnitToAdd);
	void RemoveUnitFromTurnManaging(AUnit* UnitToRemove);

#pragma endregion

#pragma region AttackManaging

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Attack", Meta = (AllowPrivateAccess = "true"))
		TArray<FAttackOrder> OrderToPlay;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Attack", Meta = (AllowPrivateAccess = "true"))
		FAttackOrder CurrentAttackOrder;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Attack", Meta = (AllowPrivateAccess = "true"))
		bool bAttackerWaiting = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Attack", Meta = (AllowPrivateAccess = "true"))
		bool bDefenderWaiting = false;

	UPROPERTY()
		UUnitAttackActionComponent* CurrentAttackActionComponent;

	//UPROPERTY()
	//	UUnitAttackActionComponent* AttackerAttackActionComponent;

	//UPROPERTY()
	//	UUnitAttackActionComponent* DefenderAttackActionComponent;

public:

	void SetupAttackManaging(AActor* Attacker, AActor* Defender);

	void StartAttack();
	void PlayAttack();
	void FinishAttack();

	UFUNCTION()
		void OnAttackHit();
	UFUNCTION()
		void OnAttackEnd();
	UFUNCTION()
		void OnHit();
	UFUNCTION()
		void OnHitEnd();

	TArray<FAttackOrder> CalculateAttackOrder(AActor* Attacker, AActor* Defender);
	float CalculateCriticalRate(AActor* Attacker, AActor* Defender);
	float CalculateAccuracy(AActor* Attacker, AActor* Defender);

	void BindOnAttackHit(UUnitAnimInstance* AnimInst);
	void BindOnAttackEnd(UUnitAnimInstance* AnimInst);
	void BindOnHit(UUnitAnimInstance* AnimInst);
	void BindOnHitEnd(UUnitAnimInstance* AnimInst);

	void TryPlayNextOrder();

	TArray<FAttackOrder> GetAttackOrder() const;

#pragma endregion

};
