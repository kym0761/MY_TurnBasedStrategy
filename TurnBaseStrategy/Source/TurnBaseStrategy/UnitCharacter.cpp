// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitCharacter.h"
#include "StatComponent.h"
#include "UnitMoveActionComponent.h"

#include "Kismet/GameplayStatics.h"
#include "GridManager.h"

// Sets default values
AUnitCharacter::AUnitCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StatComponent = CreateDefaultSubobject<UStatComponent>("StatComponent");

	UnitMoveActionComponent = CreateDefaultSubobject<UUnitMoveActionComponent>("UnitMoveActionComponent");
}

// Called when the game starts or when spawned
void AUnitCharacter::BeginPlay()
{
	Super::BeginPlay();


	AGridManager* gridManager = Cast<AGridManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGridManager::StaticClass()));
	if (IsValid(gridManager))
	{
		Grid= gridManager->WorldToGrid(GetActorLocation());
		gridManager->AddUnitAtGrid(Grid, this);
	}

}

// Called every frame
void AUnitCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AUnitCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float AUnitCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (IsValid(StatComponent))
	{
		StatComponent->DealDamage(DamageAmount);
	}

	return 0.0f;
}

FGrid AUnitCharacter::GetGrid()
{
	return Grid;
}

void AUnitCharacter::SetGrid(FGrid GridValue)
{
	Grid = GridValue;
}

