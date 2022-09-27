// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitCharacter.h"
#include "StatComponent.h"

#include "UnitMoveActionComponent.h"
#include "UnitAttackActionComponent.h"


#include "Kismet/GameplayStatics.h"
#include "GridManager.h"
#include "TurnManager.h"
#include "UnitSelectPawn.h"

// Sets default values
AUnitCharacter::AUnitCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StatComponent = CreateDefaultSubobject<UStatComponent>("StatComponent");

	UnitMoveActionComponent = CreateDefaultSubobject<UUnitMoveActionComponent>("UnitMoveActionComponent");
	UnitAttackActionComponent = CreateDefaultSubobject<UUnitAttackActionComponent>("UnitAttackActionComponent");
}

// Called when the game starts or when spawned
void AUnitCharacter::BeginPlay()
{
	Super::BeginPlay();


	AGridManager* gridManager = AGridManager::GetGridManager();
	if (IsValid(gridManager))
	{
		Grid= gridManager->WorldToGrid(GetActorLocation());
		gridManager->AddUnitAtGrid(this, Grid);
	}

	ATurnManager* turnManager = ATurnManager::GetTurnManager();
	if (IsValid(turnManager))
	{
		turnManager->OnTurnChanged.AddDynamic(this, &AUnitCharacter::OnTurnChanged);
	}

	AUnitSelectPawn* unitSelectPawn = AUnitSelectPawn::GetUnitSelectPawn(); 
	if (IsValid(unitSelectPawn))
	{
		unitSelectPawn->OnSelectedUnitChanged.AddDynamic(this, &AUnitCharacter::OnSelectedUnitChanged);
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

UUnitActionComponent* AUnitCharacter::GetUnitActionComponent(EUnitActionType UnitActionType)
{
	switch (UnitActionType)
	{
	case EUnitActionType::Move:
		return UnitMoveActionComponent;

	case EUnitActionType::Attack:
		return UnitAttackActionComponent;
	default:
		return nullptr;
	}
}

void AUnitCharacter::OnTurnChanged()
{
	//Function when TurnChanged.
	UE_LOG(LogTemp, Warning, TEXT("OnTurnChanged -> %s"), *GetActorLabel());
}

bool AUnitCharacter::HasActionComponent(EUnitActionType UnitActionType)
{
	switch (UnitActionType)
	{
	case EUnitActionType::Move:
		return IsValid(UnitMoveActionComponent);

	case EUnitActionType::Attack:
		return IsValid(UnitAttackActionComponent);

	default:
		return false;
	}
}

void AUnitCharacter::OnSelectedUnitChanged()
{	
	AUnitSelectPawn* unitSelectPawn = AUnitSelectPawn::GetUnitSelectPawn();
	if (IsValid(unitSelectPawn))
	{
		if (unitSelectPawn->GetSelectedUnit() == this)
		{
			UE_LOG(LogTemp, Warning, TEXT("OnSelectedUnitChanged -> %s"), *GetActorLabel());
		}
	}
}

