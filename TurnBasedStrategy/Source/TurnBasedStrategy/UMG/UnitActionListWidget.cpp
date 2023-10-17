// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitActionListWidget.h"
#include "UnitCore/Unit.h"
#include "UnitAction/UnitActionComponent.h"
#include "ActionSelectButtonWidget.h"
#include "Components/VerticalBox.h"

void UUnitActionListWidget::InitUnitActionsWidget(AUnit* SelectedUnit)
{
	//1. ������ Action�� ���� �˻�
	//2. ������ Action�� Ȯ��
	//3. ������ Action�� ���� ��ư ����.

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

		//�� �׼��� �̹� �Ͽ� ����� ��������
		if (!unitAction_Cast->IsCanDoActionThisTurn())
		{
			continue;
		}

		//�� �׼��� ���� ��ġ���� ��밡���� ĭ�� �����ϴ���
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

		//��ư �߰�
		VerticalBox_ActionList->AddChild(buttonWidget);
		buttonWidget->InitActionSelectButton(unitAction_Cast);
		buttonWidget->OnButtonClickedCompleted.AddDynamic(
			this, &UUnitActionListWidget::OnButtonClickedCompletedFunc);
	}

	//ù��° ��ư�� Focus
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