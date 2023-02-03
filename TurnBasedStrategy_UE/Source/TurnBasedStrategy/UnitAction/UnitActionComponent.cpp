// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitActionComponent.h"
#include "../UnitCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "../UnitSelectPawn.h"


// Sets default values for this component's properties
UUnitActionComponent::UUnitActionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	MaxActionRange = 0;
	bCanAction = true;

	ActionName = FString("BaseAction");

}


// Called when the game starts
void UUnitActionComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

	Unit = Cast<AUnitCharacter>(GetOwner());

	 OnActionStart.AddDynamic(this,&UUnitActionComponent::OnActionStartFunc);
	 OnActionEnd.AddDynamic(this,&UUnitActionComponent::OnActionEndFunc);
	 OnActionSelected.AddDynamic(this, &UUnitActionComponent::OnActionSelectedFunc);
}


// Called every frame
void UUnitActionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

FString UUnitActionComponent::GetActionName() const
{
	return ActionName;
}

void UUnitActionComponent::TakeAction(FGrid Grid)
{
	//Do Nothing?
	return;
}

bool UUnitActionComponent::IsValidActionGrid(FGrid Grid) const
{
	return GetValidActionGridArray().Contains(Grid);
}

TArray<FGridVisualData> UUnitActionComponent::GetValidActionGridVisualDataArray() const
{
	FGridVisualData data;
	data.Grid = Unit->GetGrid();
	data.GridVisualType = EGridVisualType::OK;
	TArray<FGridVisualData> temp;
	temp.Add(data);

	return temp;
}

TArray<FGrid> UUnitActionComponent::GetValidActionGridArray() const
{
	TArray<FGrid> temp;
	temp.Add(Unit->GetGrid());
	return temp;
}

AUnitCharacter* UUnitActionComponent::GetUnit() const
{
	return Unit;
}

bool UUnitActionComponent::ThisActionCanBeDo() const
{
	return bCanAction;
}

void UUnitActionComponent::SetCanAction(bool InputBool)
{
	bCanAction = InputBool;
}

void UUnitActionComponent::OnActionStartFunc()
{
	APlayerController* playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	AUnitSelectPawn* pawn = Cast<AUnitSelectPawn>(playerController->GetPawn());
	pawn->SetIsBusy(true);
}

void UUnitActionComponent::OnActionEndFunc()
{
	APlayerController* playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	AUnitSelectPawn* pawn = Cast<AUnitSelectPawn>(playerController->GetPawn());
	pawn->SetIsBusy(false);

	bCanAction = false;
}

void UUnitActionComponent::OnActionSelectedFunc()
{

}

