// Fill out your copyright notice in the Description page of Project Settings.


#include "WaitActionComponent.h"
#include "Kismet/GameplayStatics.h"
#include "../UnitSelectPawn.h"

UWaitActionComponent::UWaitActionComponent()
{
	ActionName = "Wait";
}

void UWaitActionComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UWaitActionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UWaitActionComponent::TakeAction(FGrid Grid)
{
	TArray<UActorComponent*> unitActions;
	GetOwner()->GetComponents(UUnitActionComponent::StaticClass(), unitActions);

	for (auto unitAction : unitActions)
	{
		auto unitAction_Cast =
			Cast<UUnitActionComponent>(unitAction);

		if (!IsValid(unitAction_Cast))
		{
			continue;
		}

		unitAction_Cast->SetCanAction(false);
	}

	APlayerController* playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	AUnitSelectPawn* pawn = Cast<AUnitSelectPawn>(playerController->GetPawn());
	pawn->DeSelect();

	//if (OnActionEnd.IsBound())
	//{
	//	OnActionEnd.Broadcast();
	//}

}

void UWaitActionComponent::OnActionSelectedFunc()
{
	TakeAction(FGrid());
}
