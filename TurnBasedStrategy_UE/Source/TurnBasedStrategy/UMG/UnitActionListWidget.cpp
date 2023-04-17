// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitActionListWidget.h"
#include "UnitCore/Unit.h"
#include "UnitAction/UnitActionComponent.h"
#include "ActionSelectButtonWidget.h"
#include "Components/VerticalBox.h"

void UUnitActionListWidget::InitUnitActionsWidget(AUnit* SelectedUnit)
{
	//1. 유닛의 Action을 전부 검색
	//2. 가능한 Action을 확인
	//3. 가능한 Action에 대한 버튼 생성.

	if (!IsValid(SelectedUnit))
	{
		return;
	}

	TArray<UActorComponent*> unitActions;
	SelectedUnit->GetComponents(UUnitActionComponent::StaticClass(), unitActions);

	for (UActorComponent* unitAction : unitActions)
	{
		UUnitActionComponent* unitAction_Cast =
			Cast<UUnitActionComponent>(unitAction);

		if (!IsValid(unitAction_Cast))
		{
			continue;
		}

		//이 액션이 이번 턴에 사용이 가능한지
		if (!unitAction_Cast->IsCanDoActionThisTurn())
		{
			continue;
		}

		//이 액션이 현재 위치에서 사용가능한 칸이 존재하는지
		if (!unitAction_Cast->IsCurrentlyAvailableAction())
		{
			continue;
		}

		UActionSelectButtonWidget* buttonWidget =
			CreateWidget<UActionSelectButtonWidget>(GetWorld(), ChooseActionButtonWidgetClass);

		if (!IsValid(buttonWidget))
		{
			continue;
		}

		//버튼 추가
		VerticalBox_ActionList->AddChild(buttonWidget);
		buttonWidget->InitActionSelectButton(unitAction_Cast);
		buttonWidget->OnButtonClickedCompleted.AddDynamic(
			this, &UUnitActionListWidget::OnButtonClickedCompletedFunc);
	}

	//첫번째 버튼에 Focus
	if (VerticalBox_ActionList->GetChildrenCount() > 0)
	{
		auto firstButton = Cast<UActionSelectButtonWidget>(VerticalBox_ActionList->GetChildAt(0));
		if (IsValid(firstButton))
		{
			firstButton->SetFocusThisButton();
		}
	}

}

void UUnitActionListWidget::OnButtonClickedCompletedFunc()
{
	RemoveFromParent();
}
