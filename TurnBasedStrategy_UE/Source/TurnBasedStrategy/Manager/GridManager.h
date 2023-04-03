// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Grid/Grid.h"
#include "Grid/GridVisual.h"
#include "GridManager.generated.h"

class AGridVisual;
class UGridObject;
class UPathNode;
class AUnitCharacter;
class UBillboardComponent;
class UInstancedGridVisualComponent;
class UPathFindingSystem;
class UGridSystem;

/* 
 * GridSystem과 PathFindingSystem의 조화를 위해서
 * 각 System은 각각의 UObject만 관리하고
 * PathFinding같은 기능은 GridManager에서 관리해야함.
*/

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAnyUnitMoved);

UCLASS()
class TURNBASEDSTRATEGY_API AGridManager : public AActor
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

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = true))
	int32 X_Length = 10;
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = true))
	int32 Y_Length = 10;
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = true))
	float CellSize = 100.0f;

	/*
	* GridVisual
	* 이 Instanced Component들은 선택할 수 있을 만한 Grid의 정보들을 Visual로 보여주는 역할을 한다.
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

#pragma endregion

public:	
	// Sets default values for this actor's properties
	AGridManager();

	FOnAnyUnitMoved OnAnyUnitMoved;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//virtual void OnConstruction(const FTransform& Transform);

	//Init
	void CreateGridSystem();

public:	

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/*GridVisual Functions*/
	void ShowGridRange(const FGrid& Grid, int32 Range, EGridVisualType GridVisualType);
	void ShowFromGridSet(const TSet<FGrid>& GridSet, EGridVisualType GridVisualType, const float Height = 0.01f);
	void ShowFromGridVisualDataSet(const TSet<FGridVisualData>& GridVisualDataSet, const float Height = 0.01f);
	void RemoveAllGridVisual();

	/*PathFinding Functions*/
	TArray<FGrid> FindPath(const FGrid& Start, const FGrid& End, int32& PathLength, bool bCanIgnoreUnit = false);
	int32 CalculateGridDistance(const FGrid& a, const FGrid& b) const;
	UPathNode* GetLowestFCostNode(TArray<UPathNode*>& PathNodeList);
	TArray<FGrid> CalculatePath(UPathNode* EndNode) const;
	TArray<UPathNode*> GetNearNodeArray(UPathNode* CurrentNode) const;
	bool HasPath(const FGrid& Start, const FGrid& End, bool bCanIgnoreUnit = false);
	void InitAllPathFindingNodes();

	/*Grid System Functions*/
	TArray<AUnitCharacter*> GetUnitArrayAtGrid(const FGrid& GridValue) const;
	TArray<AUnitCharacter*> GetAllUnitInGridSystem() const;
	AUnitCharacter* GetUnitAtGrid(const FGrid& GridValue) const;
	bool HasAnyUnitOnGrid(const FGrid& GridValue) const;
	bool IsWalkableGrid(const FGrid& GridValue) const;
	int32 GetPathLength(const FGrid& Start, const FGrid& End);
	bool IsValidGrid(const FGrid& Grid) const;
	FGrid WorldToGrid(const FVector& WorldPosition) const;
	FVector GridToWorld(const FGrid& Grid) const;
	UGridObject* GetValidGridObject(const FGrid& Grid) const;
	void AddUnitAtGrid(AUnitCharacter* Unit, const FGrid& GridValue);
	void RemoveUnitAtGrid(AUnitCharacter* Unit, const FGrid& GridValue);
	void MoveUnitGrid(AUnitCharacter* Unit, const FGrid& From, const FGrid& to);
	TMap<FGrid, UGridObject*> GetAllGridObjectsThatHasUnit() const;



	static AGridManager* GetGridManager();

	//Test EnemyRangeFunction
	//현재 BP_GridManager에서 R버튼 입력에 대해 Bind된 상태로 동작함.
	//추후에 변경할 예정.
	UFUNCTION(BlueprintCallable)
		void ShowEnemyRange();

	UFUNCTION(BlueprintCallable)
		void ClearEnemyRange();
};
