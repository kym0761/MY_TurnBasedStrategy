// Fill out your copyright notice in the Description page of Project Settings.


#include "WaitActionComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UnitControl/UnitSelectPawn.h"

UWaitActionComponent::UWaitActionComponent()
{
	ActionName = TEXT("Wait");

	MaxActionRange = 0;
}

void UWaitActionComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UWaitActionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UWaitActionComponent::TakeAction(const FGrid& Grid)
{
	TArray<UActorComponent*> unitActions;
	GetOwner()->GetComponents(UUnitActionComponent::StaticClass(), unitActions);

	for (UActorComponent* unitAction : unitActions)
	{
		UUnitActionComponent* unitAction_Cast =
			Cast<UUnitActionComponent>(unitAction);

		if (!IsValid(unitAction_Cast))
		{
			continue;
		}

		unitAction_Cast->SetCanDoActionThisTurn(false);
	}

	ActionEnd();

}

void UWaitActionComponent::DealWithGridBeforeAction(const FGrid& Grid)
{
	TakeAction(Grid);
}

void UWaitActionComponent::ActionSelected()
{
	Super::ActionSelected();

	//WaitAction�� ��ư�� ���õǸ� �ű⼭ ���� ��.
	//�׷��Ƿ� �ٸ� Action�� �ٸ��� Select���ڸ��� �ٷ� �ߵ��ؾ���. FGrid()�� ��ǻ� �ʿ����.
	DealWithGridBeforeAction(FGrid());
}

