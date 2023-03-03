// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyControlPawn.h"

// Sets default values
AEnemyControlPawn::AEnemyControlPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEnemyControlPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemyControlPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemyControlPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

