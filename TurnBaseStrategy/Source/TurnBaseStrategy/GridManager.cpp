// Fill out your copyright notice in the Description page of Project Settings.


#include "GridManager.h"


// Sets default values
AGridManager::AGridManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bRunConstructionScriptOnDrag = false;
}

// Called when the game starts or when spawned
void AGridManager::BeginPlay()
{
	Super::BeginPlay();
}

void AGridManager::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	//UE_LOG(LogTemp, Warning, TEXT("OnConstruction"));

	//if (GridSystem.IsValid())
	//{
	//	GridSystem.Reset();
	//}

	//auto gridsystem = new FGridSystem<UGridObject>(X_Length, Y_Length, CellSize, [=](const FGridSystem<UGridObject>* g, FGrid grid) -> UGridObject* {
	//	UGridObject* gridObj = NewObject<UGridObject>();
	//	//gridObj->SetGrid(grid);
	//	//gridObj->;
	//	return gridObj;
	//	});

	//GridSystem = MakeShareable(gridsystem);


	//bool isGood = true;
	//if (GridObjectArray.Num() == (X_Length * Y_Length))
	//{
	//	for (auto gridobject : GridObjectArray)
	//	{
	//		if (IsValid(gridobject))
	//		{
	//			continue;
	//		}
	//		else
	//		{
	//			isGood = false;
	//			break;
	//		}
	//	}
	//}
	//else
	//{
	//	isGood = false;
	//}

	//if (isGood == false)
	//{
	//	GridObjectArray.Empty();

	//	for (int32 x = 0; x < X_Length; x++)
	//	{
	//		for (int32 y = 0; y < Y_Length; y++)
	//		{
	//			FGrid grid;
	//			grid.X = x;
	//			grid.Y = y;

	//			//TODO : Height?

	//			UGridObject* gridObject = NewObject<UGridObject>();
	//			gridObject->SetGrid(grid);
	//			GridObjectArray.Add(gridObject);
	//		}
	//	}
	//}
}

// Called every frame
void AGridManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}



