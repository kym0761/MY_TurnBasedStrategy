// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridCostModifier.generated.h"

UCLASS()
class TURNBASEDSTRATEGY_API AGridCostModifier : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGridCostModifier();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cost")
	int32 CostValue;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
