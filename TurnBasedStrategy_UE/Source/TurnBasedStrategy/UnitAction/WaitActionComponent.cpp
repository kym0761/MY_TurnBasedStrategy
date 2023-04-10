// Fill out your copyright notice in the Description page of Project Settings.


#include "WaitActionComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UnitCore/UnitCharacter.h"
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

	UE_LOG(LogTemp, Warning, TEXT("Wait OK"));

	ActionEnd();

	auto owner = Cast<AUnitCharacter>(GetOwner());
	if (IsValid(owner))
	{
		owner->FinishUnitAllAction();
	}

}

void UWaitActionComponent::DealWithGridBeforeAction(const FGrid& Grid)
{
	TakeAction(Grid);
}

void UWaitActionComponent::ActionSelected()
{
	Super::ActionSelected();

	//WaitAction은 버튼이 선택되면 거기서 끝이 남.
	//그러므로 다른 Action과 다르게 Select되자마자 바로 발동해야함. FGrid()는 사실상 필요없음.
	DealWithGridBeforeAction(FGrid());
}

