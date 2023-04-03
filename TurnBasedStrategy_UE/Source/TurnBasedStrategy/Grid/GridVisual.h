// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Grid.h"
#include "GridVisual.generated.h"

class UStaticMeshComponent;
class USceneComponent;

/*
* Deprecated
* 사유 : GridVisual Actor를 Grid 갯수만큼 배치하는 것보다 InstancedMesh를 사용해 비쥬얼을 보여주는 것이 더 Cost가 저렴함.
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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Show();
	void Hide();
};
