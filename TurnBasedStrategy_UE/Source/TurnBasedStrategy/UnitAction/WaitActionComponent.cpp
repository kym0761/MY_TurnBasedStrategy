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

	//WaitAction�� ��ư�� ���õǸ� �ű⼭ ���� ��.
	//�׷��Ƿ� �ٸ� Action�� �ٸ��� Select���ڸ��� �ٷ� �ߵ��ؾ���. FGrid()�� ��ǻ� �ʿ����.
	DealWithGridBeforeAction(FGrid());
}

