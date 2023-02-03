// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitActionListWidget.h"
#include "../UnitCharacter.h"
#include "../UnitAction/UnitActionComponent.h"
#include "ActionSelectButtonWidget.h"
#include "Components/VerticalBox.h"

void UUnitActionListWidget::InitUnitActionsWidget(AUnitCharacter* SelectedCharacter)
{
	if (!IsValid(SelectedCharacter))
	{
		return;
	}

	TArray<UActorComponent*> unitActions;

	SelectedCharacter->GetComponents(UUnitActionComponent::StaticClass(), unitActions);
	//auto unitActions = SelectedCharacter->GetComponentsByClass(UUnitActionComponent::StaticClass());

	for (auto* unitAction : unitActions)
	{
		auto unitAction_Cast=
		Cast<UUnitActionComponent>(unitAction);

		if (!IsValid(unitAction_Cast))
		{
			continue;
		}

		if (!unitAction_Cast->ThisActionCanBeDo())
		{
			continue;
		}

		auto buttonWidget = CreateWidget<UActionSelectButtonWidget>(GetWorld(), ChooseActionButtonWidgetClass);
		
		if (!IsValid(buttonWidget))
		{
			continue;
		}

		VerticalBox_ActionList->AddChild(buttonWidget);
		buttonWidget->InitActionSelectButton(unitAction_Cast);
		buttonWidget->OnButtonClickedCompleted.AddDynamic(this, &UUnitActionListWidget::OnButtonClickedCompletedFunc);

		//buttonWidget->OnButtonClickedCompleted.RemoveDynamic(this, &UUnitActionListWidget::OnButtonClickedCompletedFunc);
	}
}

void UUnitActionListWidget::OnButtonClickedCompletedFunc()
{
	RemoveFromParent();
}
