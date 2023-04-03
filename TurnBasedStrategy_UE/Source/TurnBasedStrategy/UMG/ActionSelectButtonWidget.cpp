// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionSelectButtonWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "UnitAction/UnitActionComponent.h"
#include "Kismet/GameplayStatics.h"
//#include "UnitControl/UnitSelectPawn.h"
#include "Manager/GridManager.h"
#include "UnitControl/UnitControlPawn.h"

UActionSelectButtonWidget::UActionSelectButtonWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsFocusable = true;
}

void UActionSelectButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();

}

void UActionSelectButtonWidget::InitActionSelectButton(UUnitActionComponent* InputAction)
{
	//눌렀을 때 활성화될 Action을 등록

	if (IsValid(InputAction))
	{
		UnitAction = InputAction;
	}

	InitButton();

}

void UActionSelectButtonWidget::InitButton()
{
	if (IsValid(Button_Select))
	{
		Button_Select->OnClicked.AddDynamic(this, &UActionSelectButtonWidget::OnButtonClicked);
	}

	if (IsValid(TextBlock_ActionName))
	{
		if (IsValid(UnitAction))
		{
			TextBlock_ActionName->SetText(FText::FromString(UnitAction->GetActionName()));
		}
	}
}

void UActionSelectButtonWidget::OnButtonClicked()
{
	//원하는 Action에 대한 버튼을 눌렀을 시에
	//ControlPawn에 UnitAction을 선택하고 Action 가능한 Grid들을 보여주게 될 것.


	if (!IsValid(UnitAction))
	{
		return;
	}

	APlayerController* playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (IsValid(playerController))
	{
		AUnitControlPawn* pawn = Cast<AUnitControlPawn>(playerController->GetPawn());

		if (IsValid(pawn))
		{
			pawn->SetSelectedAction(UnitAction);
		}
		else
		{
			return;
		}
	}

	AGridManager* gridManager = AGridManager::GetGridManager();
	if (IsValid(gridManager))
	{
		gridManager->RemoveAllGridVisual();
		gridManager->ShowFromGridVisualDataSet(UnitAction->GetValidActionGridVisualDataSet());
	}

	if (OnButtonClickedCompleted.IsBound())
	{
		OnButtonClickedCompleted.Broadcast();
	}

}
