// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Grid.h"
#include "GridVisual.h"
#include "GridManager.generated.h"

class AGridVisual;
class UGridObject;
class UPathNode;
class AUnitCharacter;
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
	
private:

	UPROPERTY()
	UGridSystem* GridSystem;
	
	UPROPERTY()
	UPathFindingSystem* PathFindingSystem;

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = true))
	int32 X_Length = 10;
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = true))
	int32 Y_Length = 10;
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = true))
	float CellSize = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Grid", Meta = (AllowPrivateAccess = true))
		UInstancedGridVisualComponent* GridVisual_Move;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Grid", Meta = (AllowPrivateAccess = true))
		UInstancedGridVisualComponent* GridVisual_OK;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Grid", Meta = (AllowPrivateAccess = true))
		UInstancedGridVisualComponent* GridVisual_NO;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Grid", Meta = (AllowPrivateAccess = true))
		UInstancedGridVisualComponent* GridVisual_Warning;

public:	
	// Sets default values for this actor's properties
	AGridManager();

	FOnAnyUnitMoved OnAnyUnitMoved;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//virtual void OnConstruction(const FTransform& Transform);
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	bool IsValidGrid(const FGrid& Grid);

	void CreateGridSystem();

	void RemoveAllGridVisual();

	FGrid WorldToGrid(const FVector& WorldPosition);
	FVector GridToWorld(const FGrid& Grid);

	UGridObject* GetValidGridObject(const FGrid& Grid);

	void ShowGridRange(const FGrid& Grid, int32 Range, EGridVisualType GridVisualType);
	void ShowFromGridArray(const TArray<FGrid>& GridArray, EGridVisualType GridVisualType);
	void ShowFromGridVisualDataArray(const TArray<FGridVisualData>& GridVisualDataArray);


	TArray<FGrid> FindPath(const FGrid& Start, const FGrid& End, int32& PathLength, bool bCanIgnoreUnit = false);
	int32 CalculateGridDistance(const FGrid& a, const FGrid& b);
	UPathNode* GetLowestFCostNode(const TArray<UPathNode*>& PathNodeList);
	TArray<FGrid> CalculatePath(UPathNode* EndNode);
	TArray<UPathNode*> GetNearNodeArray(UPathNode* CurrentNode);

	TArray<AUnitCharacter*> GetUnitArrayAtGrid(const FGrid& GridValue);
	AUnitCharacter* GetUnitAtGrid(const FGrid& GridValue);
	bool HasAnyUnitOnGrid(const FGrid& GridValue);
	bool HasPath(const FGrid& Start, const FGrid& End, bool bCanIgnoreUnit = false);
	bool IsWalkableGrid(const FGrid& GridValue);
	int32 GetPathLength(const FGrid& Start, const FGrid& End);

	void InitAllPathFindingNodes();

	static AGridManager* GetGridManager();

	void AddUnitAtGrid(AUnitCharacter* Unit, const FGrid& GridValue);
	void RemoveUnitAtGrid(AUnitCharacter* Unit, const FGrid& GridValue);
	void MoveUnitGrid(AUnitCharacter* Unit, const FGrid& From, const FGrid& to);

};
