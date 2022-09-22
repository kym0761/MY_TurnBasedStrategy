// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridVisual.generated.h"

class UStaticMeshComponent;
class USceneComponent;

UENUM(BlueprintType)
enum class EGridVisualType : uint8
{
	White UMETA(DisplayName = "White"),
	Blue UMETA(DisplayName = "Blue"),
	Red UMETA(DisplayName = "Red"),
	RedSoft UMETA(DisplayName = "RedSoft"),
	Yellow UMETA(DisplayName = "Yellow")

};

USTRUCT(BlueprintType)
struct FGridVisualTypeMaterial
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
		EGridVisualType GridVisualType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
		UMaterialInstance* Material;
};


UCLASS(abstract)
class TURNBASESTRATEGY_API AGridVisual : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGridVisual();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
		USceneComponent* Scene;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
		UStaticMeshComponent* GridMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
		TArray<FGridVisualTypeMaterial> GridVisualTypeList;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Show();
	void Hide();
};
