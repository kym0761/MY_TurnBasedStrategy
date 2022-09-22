// Fill out your copyright notice in the Description page of Project Settings.


#include "GridManager.h"


// Sets default values
AGridManager::AGridManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGridManager::BeginPlay()
{
	Super::BeginPlay();
	
	auto gridsystem = new FGridSystem<UGridObject>(10, 10, 100.0f, &AGridManager::CreateGrid);


}

// Called every frame
void AGridManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

UGridObject* AGridManager::CreateGrid(FGridSystem<UGridObject> gs, FGrid grid)
{
	UGridObject* go = NewObject<UGridObject>();
	return go;
}

