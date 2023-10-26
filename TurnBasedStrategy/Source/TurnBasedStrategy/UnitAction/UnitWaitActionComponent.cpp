// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitWaitActionComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UnitCore/Unit.h"

#include "DebugHelper.h"

UUnitWaitActionComponent::UUnitWaitActionComponent()
{
	ActionName = TEXT("Wait");

	MaxActionRange = 0;
}

void UUnitWaitActionComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UUnitWaitActionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UUnitWaitActionComponent::TakeAction(const FGrid& Grid)
{

	Debug::Print(DEBUG_TEXT("Wait OK"));

	ActionEnd();

	auto owner = Cast<AUnit>(GetOwner());
	if (IsValid(owner))
	{
		owner->FinishUnitAllAction();
	}

}

void UUnitWaitActionComponent::DealWithGridBeforeAction(const FGrid& Grid)
{
	TakeAction(Grid);
}

void UUnitWaitActionComponent::ActionSelected()
{
	Super::ActionSelected();

	//WaitAction은 버튼이 선택되면 거기서 끝이 남.
	//그러므로 다른 Action과 다르게 Select되자마자 바로 발동해야함. FGrid()는 사실상 필요없음.
	DealWithGridBeforeAction(FGrid());
}

