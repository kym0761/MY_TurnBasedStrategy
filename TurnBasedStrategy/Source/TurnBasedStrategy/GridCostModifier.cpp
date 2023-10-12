// Fill out your copyright notice in the Description page of Project Settings.


#include "GridCostModifier.h"

// Sets default values
AGridCostModifier::AGridCostModifier()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CostValue = 3;
}

// Called when the game starts or when spawned
void AGridCostModifier::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGridCostModifier::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

