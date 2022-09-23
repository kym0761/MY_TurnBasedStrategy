// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Grid.h"
#include "GridSystem.h"
#include "GridVisual.h"
#include "GridManager.generated.h"

class AGridVisual;
class UGridObject;
class UPathNode;



UCLASS()
class TURNBASESTRATEGY_API AGridManager : public AActor
{
	GENERATED_BODY()
	
private:

	TSharedPtr<FGridSystem<UGridObject>> GridSystem;
	TSharedPtr<FGridSystem<UPathNode>> PathFindingGridSystem;

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = true))
	int32 X_Length = 10;
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = true))
	int32 Y_Length = 10;
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = true))
	float CellSize = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Grid", Meta = (AllowPrivateAccess = true))
		TSubclassOf<AGridVisual> GridVisualClass;

	TArray<AGridVisual*> GridVisualArray;
public:	
	// Sets default values for this actor's properties
	AGridManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform);
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	bool IsValidGrid(FGrid Grid);

	void CreateGridSystem();

	void CreateGridVisual();
	void ShowAllGridVisual();
	void HideAllGridVisual();

	FGrid WorldToGrid(FVector WorldPosition);
	FVector GridToWorld(FGrid Grid);

	UGridObject* GetValidGridObject(FGrid Grid);
	AGridVisual* GetValidGridVisual(FGrid Grid);

	void ShowGridRange(FGrid Grid, int32 Range, EGridVisualType GridVisualType);
	void ShowFromGridList(TArray<FGrid> GridList, EGridVisualType GridVisualType);

	TArray<FGrid> FindPath(FGrid Start, FGrid End, int32& PathLength);
	int32 CalculateGridDistance(FGrid a, FGrid b);
	UPathNode* GetLowestFCostNode(TArray<UPathNode*> PathNodeList);
	TArray<FGrid> CalculatePath(UPathNode* EndNode);
	TArray<UPathNode*> GetNearNodeList(UPathNode* CurrentNode);
};
