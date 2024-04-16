// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyDelegates.h"
#include "GameFramework/Actor.h"
#include "Grid/Grid.h"
#include "Interface/GridManagerInterface.h"
#include "GridManager.generated.h"

class UGridObject;
class UPathObject;
class AUnit;
class UBillboardComponent;
class UInstancedGridVisualComponent;
class UPathFindingSystem;
class UGridSystem;

class ASRPG_GameMode;

/* 
 * GridSystem과 PathFindingSystem의 조화를 위해서
 * 각 System은 각각의 UObject만 관리하고
 * PathFinding같은 기능은 GridManager에서 관리해야함.
*/

UCLASS()
class TURNBASEDSTRATEGY_API AGridManager : public AActor, public IGridManagerInterface
{
	GENERATED_BODY()
	
#pragma region PrivateData
private:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Grid", Meta = (AllowPrivateAccess = true))
	UBillboardComponent* BillboardComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Grid", Meta = (AllowPrivateAccess = true))
	UGridSystem* GridSystem;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Grid", Meta = (AllowPrivateAccess = true))
	UPathFindingSystem* PathFindingSystem;

	/*
	* GridVisual
	* 이 Instanced Mesh Component들은 Grid의 정보를 시각적으로 보여주는 역할을 한다.
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Grid", Meta = (AllowPrivateAccess = true))
		UInstancedGridVisualComponent* GridVisual_Move;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Grid", Meta = (AllowPrivateAccess = true))
		UInstancedGridVisualComponent* GridVisual_OK;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Grid", Meta = (AllowPrivateAccess = true))
		UInstancedGridVisualComponent* GridVisual_NO;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Grid", Meta = (AllowPrivateAccess = true))
		UInstancedGridVisualComponent* GridVisual_Warning;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Grid", Meta = (AllowPrivateAccess = true))
		UInstancedGridVisualComponent* GridVisual_DANGER;

	//UPROPERTY()
	//	ASRPG_GameMode* SRPG_GameModeRef;

#pragma endregion

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
		int32 X_Length = 10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
		int32 Y_Length = 10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
		float CellSize = 100.0f;

public:	
	// Sets default values for this actor's properties
	AGridManager();

	FDynamicMulticastVoid OnAnyUnitMoved;

protected:


	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void PostInitializeComponents() override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//virtual void OnConstruction(const FTransform& Transform);

	////Init
	//void CreateGridSystem();

public:	

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetupGridSystem();
	void SetupUnitsOnGrid();
	void SetupGridCostInitially();

	/*GridSystem Functions*/
	virtual TArray<AUnit*> GetUnitArrayAtGrid(const FGrid& GridValue) const;
	virtual TArray<AUnit*> GetAllUnitInGridSystem() const;
	virtual AUnit* GetUnitAtGrid(const FGrid& GridValue) const;
	virtual bool HasAnyUnitOnGrid(const FGrid& GridValue) const;
	virtual bool IsWalkableGrid(const FGrid& GridValue) const;
	virtual bool IsValidGrid(const FGrid& Grid) const;


	UFUNCTION(BlueprintCallable)
	virtual FGrid WorldToGrid(const FVector& WorldPosition) const;
	UFUNCTION(BlueprintCallable)
	virtual FVector GridToWorld(const FGrid& Grid) const;

public:

	virtual UGridObject* GetValidGridObject(const FGrid& Grid) const;
	virtual TMap<FGrid, UGridObject*> GetAllGridObjectsThatHasUnit() const;
	virtual void AddUnitAtGrid(AUnit* Unit, const FGrid& GridValue);
	virtual void RemoveUnitAtGrid(AUnit* Unit, const FGrid& GridValue);
	virtual void MoveUnitGrid(AUnit* Unit, const FGrid& From, const FGrid& to);

public:

	/*Pathfinding Functions*/
	virtual TArray<FGrid> FindPath(const FGrid& Start, const FGrid& End, int32& PathLength, const int32 MaxMoveCost, bool bCanIgnoreUnit = false, bool bCalculateToTarget = false);
	virtual int32 CalculateGridDistance(const FGrid& a, const FGrid& b) const;
	virtual bool HasPath(const FGrid& Start, const FGrid& End, int32 MaxMoveCost = 1000, bool bCanIgnoreUnit = false);
	virtual int32 GetPathLength(const FGrid& Start, const FGrid& End, const int32 MaxMoveCost);

private:

	virtual void InitAllPathFindingObjects();
	virtual void InitPathFindingObjects(const FGrid& StartGrid, const int32 MaxMoveCost);
	virtual UPathObject* GetLowestFCostObject(TArray<UPathObject*>& PathObjectList);
	virtual TArray<FGrid> CalculatePath(UPathObject* EndObject) const;
	virtual TArray<UPathObject*> GetNearObjectArray(UPathObject* CurrentObject) const;

public:

	//*GridVisual Functions*/
	virtual void ShowGridRange(const FGrid& Grid, int32 Range, EGridVisualType GridVisualType);
	virtual void ShowFromGridSet(const TSet<FGrid>& GridSet, EGridVisualType GridVisualType, const float Height = 0.01f);
	virtual void ShowFromGridVisualDataSet(const TSet<FGridVisualData>& GridVisualDataSet, const float Height = 0.01f);
	virtual void RemoveAllGridVisual();

public:

	static AGridManager* GetGridManager();

	//Test EnemyRangeFunction
	//현재 BP_GridManager에서 R버튼 입력에 대해 Bind된 상태로 동작함.
	//추후에 변경할 예정.

	//아군 유닛이 움직였을 때 갱신기능이 필요함

	UFUNCTION(BlueprintCallable)
		void ShowEnemyRange();

	UFUNCTION(BlueprintCallable)
		void ClearEnemyRange();
};
