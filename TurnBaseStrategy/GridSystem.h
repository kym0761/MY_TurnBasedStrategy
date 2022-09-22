// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Grid.h"
#include "GridObject.h"
#include "PathNode.h"
#include "GridVisual.h"
#include "GridSystem.generated.h"


class AGridVisual;

UCLASS()
class TURNBASESTRATEGY_API AGridSystem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGridSystem();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	int32 X_Length = 10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	int32 Y_Length = 10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	float CellSize = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Grid")
		TSubclassOf<AGridVisual> GridVisualClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Grid")
		TArray<UGridObject*> GridObjectArray;

private:
	UPROPERTY()
		TArray<AGridVisual*> GridVisualArray;
	UPROPERTY()
		TArray<UPathNode*> PathNodeArray;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
public:
	bool IsValidGrid(FGrid Grid);

	int32 GetX_Length();
	int32 GetY_Length();

	FGrid WorldToGrid(FVector WorldPosition);
	FVector GridToWorld(FGrid Grid);

	UGridObject* GetValidGridObject(FGrid Grid);
	AGridVisual* GetValidGridVisual(FGrid Grid);
	//TODO : CreateDebugGrid

	void CreateGridVisual();
	void ShowAllGridVisual();
	void HideAllGridVisual();
	
	void ShowGridRange(FGrid Grid, int32 Range, EGridVisualType GridVisualType);
	void ShowFromGridList(TArray<FGrid> GridList, EGridVisualType GridVisualType);



	/*Maybe, Remake*/

	TArray<FGrid> FindPath(FGrid Start, FGrid End, int32& PathLength);

	int32 CalculateGridDistance(FGrid a, FGrid b);
	UPathNode* GetLowestFCostNode(TArray<UPathNode*> PathNodeList);
	TArray<FGrid> CalculatePath(UPathNode* EndNode);
	TArray<UPathNode*> GetNearNodeList(UPathNode* CurrentNode);
};
