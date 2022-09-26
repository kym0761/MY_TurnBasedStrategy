// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitActionComponent.h"
#include "UnitCharacter.h"

// Sets default values for this component's properties
UUnitActionComponent::UUnitActionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	MaxActionRange = 0;
	bCanAction = true;
}


// Called when the game starts
void UUnitActionComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

	Unit = Cast<AUnitCharacter>(GetOwner());

}


// Called every frame
void UUnitActionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

FString UUnitActionComponent::GetActionName() const
{
	return FString("BaseAction");
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

TArray<FGrid> UUnitActionComponent::GetValidActionGridArray() const
{
	return TArray<FGrid>();
}

AUnitCharacter* UUnitActionComponent::GetUnit() const
{
	return Unit;
}

