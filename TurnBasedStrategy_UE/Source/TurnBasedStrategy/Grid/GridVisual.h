// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Grid.h"
#include "GridVisual.generated.h"

class UStaticMeshComponent;
class USceneComponent;

/*
* Deprecated? 
*/

UCLASS(abstract)
class TURNBASEDSTRATEGY_API AGridVisual : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGridVisual();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
		USceneComponent* Scene;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
		UStaticMeshComponent* GridMesh;

private:
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual", Meta = (AllowPrivateAccess = true))
	//	TArray<FGridVisualTypeMaterial> GridVisualTypeList;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Show();
	void Hide();
};
