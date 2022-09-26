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
class TURNBASESTRATEGY_API AGridManager : public AActor
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

	bool IsValidGrid(FGrid Grid);

	void CreateGridSystem();

	void RemoveAllGridVisual();

	FGrid WorldToGrid(FVector WorldPosition);
	FVector GridToWorld(FGrid Grid);

	UGridObject* GetValidGridObject(FGrid Grid);

	void ShowGridRange(FGrid Grid, int32 Range, EGridVisualType GridVisualType);
	void ShowFromGridArray(TArray<FGrid> GridArray, EGridVisualType GridVisualType);
	void ShowFromGridVisualDataArray(TArray<FGridVisualData> GridVisualDataArray);


	TArray<FGrid> FindPath(FGrid Start, FGrid End, int32& PathLength, bool bCanIgnoreUnit = false);
	int32 CalculateGridDistance(FGrid a, FGrid b);
	UPathNode* GetLowestFCostNode(TArray<UPathNode*> PathNodeList);
	TArray<FGrid> CalculatePath(UPathNode* EndNode);
	TArray<UPathNode*> GetNearNodeArray(UPathNode* CurrentNode);

	TArray<AUnitCharacter*> GetUnitArrayAtGrid(FGrid GridValue);
	AUnitCharacter* GetUnitAtGrid(FGrid GridValue);
	bool HasAnyUnitOnGrid(FGrid GridValue);
	bool HasPath(FGrid Start, FGrid End, bool bCanIgnoreUnit = false);
	bool IsWalkableGrid(FGrid GridValue);
	int32 GetPathLength(FGrid Start, FGrid End);

	void InitAllPathFindingNodes();

	static AGridManager* GetGridManager();

	void AddUnitAtGrid(AUnitCharacter* Unit, FGrid GridValue);
	void RemoveUnitAtGrid(AUnitCharacter* Unit, FGrid GridValue);
	void MoveUnitGrid(AUnitCharacter* Unit, FGrid From, FGrid to);

};
