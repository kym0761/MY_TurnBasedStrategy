// Fill out your copyright notice in the Description page of Project Settings.


#include "GridVisual.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AGridVisual::AGridVisual()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene);

	GridMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GridMesh"));
	GridMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 1.0f));
	GridMesh->SetCollisionProfileName(TEXT("NoCollision"));
	GridMesh->SetCastShadow(false);
	GridMesh->SetupAttachment(Scene);

}

// Called when the game starts or when spawned
void AGridVisual::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGridVisual::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGridVisual::Show()
{
	GridMesh->SetVisibility(true);
}

void AGridVisual::Hide()
{
	GridMesh->SetVisibility(false);
}

